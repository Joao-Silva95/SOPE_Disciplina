#ifndef WORKER_H
#define WORKER_H

#include "loganalyzer.h"

/*
 * Analisa um único ficheiro de log e preenche 'result'.
 * Chamada dentro de cada processo filho.
 */
void process_file(const char *filepath, WorkerResult *result, AnalysisMode mode);

/*
 * Escreve o resultado no ficheiro "results_<pid>.txt".
 * Formato:
 *   PID:1234;FICHEIRO:access.log;LINHAS:50000;ERRORS:234;WARNINGS:1205
 */
void write_result_file(const WorkerResult *result);

/*
 * Imprime um relatório agregado de todos os ficheiros results_<pid>.txt
 * encontrados no diretório atual.
 * Chamada pelo processo pai depois de waitpid().
 */
void aggregate_results(int num_processes, pid_t *pids, AnalysisMode mode);

#endif /* WORKER_H */
