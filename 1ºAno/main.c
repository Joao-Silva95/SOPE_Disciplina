#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "loganalyzer.h"
#include "cli.h"
#include "worker.h"

/* ── Recolhe todos os .log do diretório ───────────────────────────────────── */
static int collect_log_files(const char *dir,
                              char files[][MAX_PATH],
                              int   max_files)
{
    DIR           *dp;
    struct dirent *entry;
    int            count = 0;

    dp = opendir(dir);
    if (!dp) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dp)) != NULL && count < max_files) {
        /* Filtra apenas ficheiros .log */
        const char *ext = strrchr(entry->d_name, '.');
        if (!ext || strcmp(ext, ".log") != 0) continue;

        snprintf(files[count], MAX_PATH, "%s/%s", dir, entry->d_name);
        count++;
    }

    closedir(dp);
    return count;
}

/* ── Função executada por cada processo filho ─────────────────────────────── */
static void child_work(char files[][MAX_PATH], int start, int end,
                       AnalysisMode mode)
{
    for (int i = start; i < end; i++) {
        WorkerResult result;
        process_file(files[i], &result, mode);
        write_result_file(&result);

        printf("[PID %d] Processado: %s (%ld linhas, %ld erros)\n",
               (int)getpid(),
               files[i],
               result.total_lines,
               result.error_count + result.critical_count);
    }
    exit(EXIT_SUCCESS);
}

/* ── main ─────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    Config cfg;
    parse_args(argc, argv, &cfg);

    if (cfg.verbose)
        print_config(&cfg);

    /* 1. Descobrir ficheiros .log */
    static char files[MAX_FILES][MAX_PATH];
    int nfiles = collect_log_files(cfg.log_dir, files, MAX_FILES);

    if (nfiles == 0) {
        fprintf(stderr, "Nenhum ficheiro .log encontrado em '%s'\n",
                cfg.log_dir);
        exit(EXIT_FAILURE);
    }

    printf("Encontrados %d ficheiro(s) .log — a criar %d processo(s)...\n\n",
           nfiles, cfg.num_processes);

    /* 2. Dividir ficheiros entre N processos filho */
    int    n   = cfg.num_processes;
    pid_t *pids = malloc(n * sizeof(pid_t));
    if (!pids) { perror("malloc"); exit(EXIT_FAILURE); }

    int base  = nfiles / n;   /* ficheiros por processo             */
    int extra = nfiles % n;   /* primeiros 'extra' processos levam +1 */

    int start = 0;
    for (int i = 0; i < n; i++) {
        int slice = base + (i < extra ? 1 : 0);
        int end   = start + slice;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            /* ── Processo filho ── */
            child_work(files, start, end, cfg.mode);
            /* child_work termina com exit(); nunca chega aqui */
        }

        /* ── Processo pai ── */
        pids[i] = pid;
        if (cfg.verbose)
            printf("[PAI] Filho %d (PID %d) — ficheiros [%d..%d)\n",
                   i + 1, (int)pid, start, end);

        start = end;
    }

    /* 3. Aguardar todos os filhos */
    printf("\n[PAI] A aguardar conclusao dos workers...\n");
    for (int i = 0; i < n; i++) {
        int   status;
        pid_t ended = waitpid(pids[i], &status, 0);
        if (ended < 0) {
            perror("waitpid");
        } else if (WIFEXITED(status)) {
            printf("[PAI] Worker PID %d terminou (exit %d)\n",
                   (int)ended, WEXITSTATUS(status));
        } else {
            printf("[PAI] Worker PID %d terminou anormalmente\n",
                   (int)ended);
        }
    }

    /* 4. Agregar e imprimir resultados */
    aggregate_results(n, pids, cfg.mode);

    /* 5. Ficheiro de saída (opcional) */
    if (cfg.output_file[0]) {
        printf("\n[PAI] Relatorio tambem disponivel em: %s\n",
               cfg.output_file);
        /* Em fases futuras redireciona o relatório para o ficheiro */
    }

    free(pids);
    return EXIT_SUCCESS;
}
