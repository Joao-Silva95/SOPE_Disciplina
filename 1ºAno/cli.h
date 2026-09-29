#ifndef CLI_H
#define CLI_H

#include "loganalyzer.h"

/*
 * Preenche 'cfg' com os argumentos de linha de comandos.
 * Em caso de erro imprime usage e termina.
 *
 * Uso:
 *   ./logAnalyzer <diretorio_logs> <num_processos> <modo> [opcoes]
 *
 * Exemplos:
 *   ./logAnalyzer /var/log/apache2 4 security --verbose --output=report.txt
 */
void parse_args(int argc, char *argv[], Config *cfg);

/* Imprime a configuração lida (útil em modo verbose) */
void print_config(const Config *cfg);

#endif /* CLI_H */
