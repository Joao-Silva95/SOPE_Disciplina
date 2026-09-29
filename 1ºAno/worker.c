#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "worker.h"

/* ── Deteção de formato ───────────────────────────────────────────────────── */
typedef enum { FMT_APACHE, FMT_JSON, FMT_SYSLOG, FMT_NGINX_ERR, FMT_UNKNOWN } LogFormat;

static LogFormat detect_format(const char *line)
{
    /* JSON começa com '{' */
    if (line[0] == '{') return FMT_JSON;

    /* Syslog começa com '<' seguido de dígitos */
    if (line[0] == '<') return FMT_SYSLOG;

    /* Nginx error: "YYYY/MM/DD HH:MM:SS [level]" */
    if (strlen(line) > 20 && line[4] == '/' && line[7] == '/') return FMT_NGINX_ERR;

    /* Apache/Combined: começa com IP */
    return FMT_APACHE;
}

/* ── Helpers de classificação ─────────────────────────────────────────────── */
static void count_level(const char *line, WorkerResult *r)
{
    if (strstr(line, "CRITICAL") || strstr(line, "crit") || strstr(line, "alert") || strstr(line, "emerg"))
        r->critical_count++;
    else if (strstr(line, "ERROR") || strstr(line, "error") || strstr(line, " [error]"))
        r->error_count++;
    else if (strstr(line, "WARN") || strstr(line, "warn") || strstr(line, "notice"))
        r->warn_count++;
    else
        r->info_count++;
}

/* Regista um IP no top-10 (inserção simples por contagem) */
static void register_ip(const char *ip, WorkerResult *r)
{
    if (ip[0] == '\0') return;

    /* Procura IP já existente */
    for (int i = 0; i < r->top_ips_count; i++) {
        if (strcmp(r->top_ips[i].ip, ip) == 0) {
            r->top_ips[i].count++;
            return;
        }
    }

    /* IP novo */
    if (r->top_ips_count < TOP_IPS) {
        strncpy(r->top_ips[r->top_ips_count].ip, ip, MAX_IP_LEN - 1);
        r->top_ips[r->top_ips_count].count = 1;
        r->top_ips_count++;
    } else {
        /* Substitui o IP com menor contagem */
        int min_idx = 0;
        for (int i = 1; i < TOP_IPS; i++)
            if (r->top_ips[i].count < r->top_ips[min_idx].count)
                min_idx = i;
        if (1 > r->top_ips[min_idx].count) {
            strncpy(r->top_ips[min_idx].ip, ip, MAX_IP_LEN - 1);
            r->top_ips[min_idx].count = 1;
        }
    }
}

/* ── Parsers por formato ──────────────────────────────────────────────────── */

/* Apache Combined: IP - - [timestamp] "METHOD url HTTP/x" STATUS bytes ... */
static void parse_apache(const char *line, WorkerResult *r)
{
    char ip[MAX_IP_LEN] = {0};
    int  status = 0;

    sscanf(line, "%63s %*s %*s %*[^]] %*c %*s %d", ip, &status);

    register_ip(ip, r);

    if (status >= 400 && status < 500) r->http_4xx++;
    else if (status >= 500)            r->http_5xx++;

    if (status >= 500)      r->error_count++;
    else if (status >= 400) r->warn_count++;
    else                    r->info_count++;
}

/* JSON: procura campos "level" e "ip" de forma simples */
static void parse_json(const char *line, WorkerResult *r)
{
    count_level(line, r);

    /* Extrai IP do campo "ip": "10.0.1.50" */
    char *p = strstr(line, "\"ip\"");
    if (p) {
        char ip[MAX_IP_LEN] = {0};
        /* Avança até às aspas do valor */
        p = strchr(p + 4, '"');
        if (p) {
            p++; /* salta a aspa de abertura */
            int i = 0;
            while (*p && *p != '"' && i < MAX_IP_LEN - 1)
                ip[i++] = *p++;
            register_ip(ip, r);
        }
    }
}

/* Syslog: <PRI>Mês DD HH:MM:SS host proc[pid]: msg */
static void parse_syslog(const char *line, WorkerResult *r)
{
    /* Prioridade: os 3 bits menos significativos são severidade */
    int pri = 0;
    sscanf(line, "<%d>", &pri);
    int severity = pri & 0x07; /* 0=emerg … 7=debug */

    if      (severity <= 2) r->critical_count++;
    else if (severity == 3) r->error_count++;
    else if (severity == 4) r->warn_count++;
    else                    r->info_count++;

    /* Tenta extrair IP de mensagens tipo "from 1.2.3.4" */
    char *p = strstr(line, " from ");
    if (p) {
        char ip[MAX_IP_LEN] = {0};
        sscanf(p + 6, "%63s", ip);
        /* Remove possível "port" ou lixo agarrado */
        char *sp = strchr(ip, ' '); if (sp) *sp = '\0';
        register_ip(ip, r);
    }
}

/* Nginx error: YYYY/MM/DD HH:MM:SS [level] pid#tid: *cid msg client: IP */
static void parse_nginx_err(const char *line, WorkerResult *r)
{
    count_level(line, r);

    char *p = strstr(line, "client: ");
    if (p) {
        char ip[MAX_IP_LEN] = {0};
        sscanf(p + 8, "%63[^,\n ]", ip);
        register_ip(ip, r);
    }
}

/* ── Leitura linha a linha usando apenas read() ───────────────────────────── */
static ssize_t read_line(int fd, char *buf, size_t maxlen)
{
    size_t n = 0;
    char   c;
    ssize_t ret;

    while (n < maxlen - 1) {
        ret = read(fd, &c, 1);
        if (ret < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (ret == 0) break; /* EOF */
        buf[n++] = c;
        if (c == '\n') break;
    }
    buf[n] = '\0';
    return (ssize_t)n;
}

/* ── process_file ─────────────────────────────────────────────────────────── */
void process_file(const char *filepath, WorkerResult *result, AnalysisMode mode)
{
    (void)mode; /* usado em fases futuras para filtrar */

    memset(result, 0, sizeof(*result));
    result->pid = getpid();
    strncpy(result->filename, filepath, MAX_PATH - 1);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    char     line[MAX_LINE];
    ssize_t  n;
    LogFormat fmt = FMT_UNKNOWN;
    int      first = 1;

    while ((n = read_line(fd, line, sizeof(line))) > 0) {
        result->total_lines++;

        /* Deteta formato na primeira linha não vazia */
        if (first && line[0] != '\n') {
            fmt   = detect_format(line);
            first = 0;
        }

        switch (fmt) {
            case FMT_APACHE:    parse_apache(line, result);    break;
            case FMT_JSON:      parse_json(line, result);      break;
            case FMT_SYSLOG:    parse_syslog(line, result);    break;
            case FMT_NGINX_ERR: parse_nginx_err(line, result); break;
            default:            count_level(line, result);     break;
        }
    }

    if (n < 0) perror("read");

    if (close(fd) < 0) perror("close");
}

/* ── write_result_file ────────────────────────────────────────────────────── */
void write_result_file(const WorkerResult *r)
{
    char fname[64];
    snprintf(fname, sizeof(fname), "results_%d.txt", (int)r->pid);

    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open result file"); return; }

    char buf[MAX_LINE];
    int  len = snprintf(buf, sizeof(buf),
        "PID:%d;FICHEIRO:%s;LINHAS:%ld;INFO:%ld;WARNINGS:%ld;ERRORS:%ld;"
        "CRITICAL:%ld;HTTP_4XX:%ld;HTTP_5XX:%ld\n",
        (int)r->pid, r->filename, r->total_lines,
        r->info_count, r->warn_count, r->error_count,
        r->critical_count, r->http_4xx, r->http_5xx);

    if (write(fd, buf, len) < 0) perror("write result");

    /* Top IPs */
    for (int i = 0; i < r->top_ips_count; i++) {
        len = snprintf(buf, sizeof(buf), "  TOP_IP:%s COUNT:%ld\n",
                       r->top_ips[i].ip, r->top_ips[i].count);
        if (write(fd, buf, len) < 0) perror("write top_ip");
    }

    if (close(fd) < 0) perror("close result file");
}

/* ── aggregate_results ────────────────────────────────────────────────────── */
void aggregate_results(int num_processes, pid_t *pids, AnalysisMode mode)
{
    long total_lines = 0, total_errors = 0, total_warnings = 0,
         total_critical = 0, total_4xx = 0, total_5xx = 0;

    const char *mode_names[] = {"SECURITY", "PERFORMANCE", "TRAFFIC", "FULL"};

    printf("\n=== RELATORIO AGREGADO [%s] ===\n\n", mode_names[mode]);

    for (int i = 0; i < num_processes; i++) {
        char fname[64];
        snprintf(fname, sizeof(fname), "results_%d.txt", (int)pids[i]);

        int fd = open(fname, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Aviso: nao encontrado %s\n", fname);
            continue;
        }

        char line[MAX_LINE];
        ssize_t n;
        while ((n = read(fd, line, sizeof(line) - 1)) > 0) {
            line[n] = '\0';
            /* Imprime cada linha do ficheiro de resultado */
            printf("%s", line);

            /* Extrai totais com sscanf para o resumo global */
            long ll = 0, er = 0, wa = 0, cr = 0, f4 = 0, f5 = 0;
            /* Extrai cada campo procurando a chave no texto */
            char *p;
            if ((p = strstr(line, "LINHAS:"))   != NULL) ll = atol(p + 7);
            if ((p = strstr(line, "WARNINGS:")) != NULL) wa = atol(p + 9);
            if ((p = strstr(line, "ERRORS:"))   != NULL) er = atol(p + 7);
            if ((p = strstr(line, "CRITICAL:")) != NULL) cr = atol(p + 9);
            if ((p = strstr(line, "HTTP_4XX:")) != NULL) f4 = atol(p + 9);
            if ((p = strstr(line, "HTTP_5XX:")) != NULL) f5 = atol(p + 9);
            if (ll > 0) {
                total_lines    += ll;
                total_errors   += er;
                total_warnings += wa;
                total_critical += cr;
                total_4xx      += f4;
                total_5xx      += f5;
            }
        }
        close(fd);
        printf("\n");
    }

    printf("─────────────────────────────────────\n");
    printf("TOTAIS GLOBAIS:\n");
    printf("  Linhas processadas : %ld\n", total_lines);
    printf("  INFO               : %ld\n",
           total_lines - total_warnings - total_errors - total_critical);
    printf("  WARNINGS           : %ld\n", total_warnings);
    printf("  ERRORS             : %ld\n", total_errors);
    printf("  CRITICAL           : %ld\n", total_critical);
    printf("  HTTP 4xx           : %ld\n", total_4xx);
    printf("  HTTP 5xx           : %ld\n", total_5xx);
    printf("─────────────────────────────────────\n");
}
