#ifndef LOGANALYZER_H
#define LOGANALYZER_H

#include <sys/types.h>

/* ── Limites ── */
#define MAX_FILES       1024
#define MAX_LINE        4096
#define MAX_IP_LEN      64
#define TOP_IPS         10
#define MAX_PATH        512

/* ── Modos de análise ── */
typedef enum {
    MODE_SECURITY,
    MODE_PERFORMANCE,
    MODE_TRAFFIC,
    MODE_FULL
} AnalysisMode;

/* ── Argumentos CLI (preenchidos em main.c) ── */
typedef struct {
    char         log_dir[MAX_PATH];
    int          num_processes;
    AnalysisMode mode;
    int          verbose;
    char         output_file[MAX_PATH]; /* vazio se não especificado */
} Config;

/* ── Par IP + contagem ── */
typedef struct {
    char ip[MAX_IP_LEN];
    long count;
} IPEntry;

/* ── Resultado produzido por cada processo filho ── */
typedef struct {
    pid_t    pid;
    char     filename[MAX_PATH];
    long     total_lines;
    long     info_count;
    long     warn_count;
    long     error_count;
    long     critical_count;
    long     http_4xx;
    long     http_5xx;
    IPEntry  top_ips[TOP_IPS];
    int      top_ips_count;
} WorkerResult;

#endif /* LOGANALYZER_H */
