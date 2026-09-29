#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

/* ── usage ────────────────────────────────────────────────────────────────── */
static void usage(const char *prog)
{
    fprintf(stderr,
        "Uso: %s <diretorio_logs> <num_processos> <modo> [opcoes]\n\n"
        "  <diretorio_logs>  Pasta com ficheiros .log\n"
        "  <num_processos>   Número de processos worker (>= 1)\n"
        "  <modo>            security | performance | traffic | full\n\n"
        "Opcoes:\n"
        "  --verbose          Modo verboso\n"
        "  --output=<file>    Ficheiro de saida do relatorio\n",
        prog);
    exit(EXIT_FAILURE);
}

/* ── parse_mode ───────────────────────────────────────────────────────────── */
static AnalysisMode parse_mode(const char *s, const char *prog)
{
    if (strcmp(s, "security")    == 0) return MODE_SECURITY;
    if (strcmp(s, "performance") == 0) return MODE_PERFORMANCE;
    if (strcmp(s, "traffic")     == 0) return MODE_TRAFFIC;
    if (strcmp(s, "full")        == 0) return MODE_FULL;

    fprintf(stderr, "Erro: modo desconhecido '%s'\n", s);
    usage(prog);
    return MODE_FULL; /* nunca chega aqui */
}

/* ── parse_args ───────────────────────────────────────────────────────────── */
void parse_args(int argc, char *argv[], Config *cfg)
{
    if (argc < 4)
        usage(argv[0]);

    /* Zerar estrutura */
    memset(cfg, 0, sizeof(*cfg));

    /* Argumentos obrigatórios */
    strncpy(cfg->log_dir, argv[1], MAX_PATH - 1);

    cfg->num_processes = atoi(argv[2]);
    if (cfg->num_processes < 1) {
        fprintf(stderr, "Erro: num_processos deve ser >= 1\n");
        usage(argv[0]);
    }

    cfg->mode = parse_mode(argv[3], argv[0]);

    /* Opções facultativas */
    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            cfg->verbose = 1;
        } else if (strncmp(argv[i], "--output=", 9) == 0) {
            strncpy(cfg->output_file, argv[i] + 9, MAX_PATH - 1);
        } else {
            fprintf(stderr, "Opcao desconhecida: %s\n", argv[i]);
            usage(argv[0]);
        }
    }
}

/* ── print_config ─────────────────────────────────────────────────────────── */
void print_config(const Config *cfg)
{
    const char *mode_names[] = {"security", "performance", "traffic", "full"};

    printf("=== Configuracao ===\n");
    printf("  Diretorio  : %s\n", cfg->log_dir);
    printf("  Processos  : %d\n", cfg->num_processes);
    printf("  Modo       : %s\n", mode_names[cfg->mode]);
    printf("  Verbose    : %s\n", cfg->verbose ? "sim" : "nao");
    if (cfg->output_file[0])
        printf("  Output     : %s\n", cfg->output_file);
    printf("====================\n\n");
}
