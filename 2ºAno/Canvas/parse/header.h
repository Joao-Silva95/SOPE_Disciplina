/*
 *  header.h
 *  parse
 *
 *  Created by Pedro Sobral on 06/10/26.
 *
 */

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXARGS 10

typedef struct command
{
	char			*name;
	char			*argv[MAXARGS];
	char			*infile;
	char			*outfile;
	char			*errfile;
	struct command	*next;
}					CMD;

CMD					*insert_command(void);
void				free_command_list(void);
void				print_command_list(void);
void				parse_line(char *);