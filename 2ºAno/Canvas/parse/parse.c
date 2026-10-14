/*
 *  parse.c
 *  parse
 *
 *  Created by Pedro Sobral on 6/10/26.
 *
 */

#include "header.h"

CMD				*command_list;

void	print_command_list(void)
{
	CMD	*temp;
	int	i;

	for (temp = command_list; temp != NULL; temp = temp->next)
	{
		printf("-----NEW COMMAND-----\n");
		printf("Name = %s\n", temp->name);
		for (i = 0; temp->argv[i] != NULL; i++)
		{
			printf("argv[%d] = %s\n", i, temp->argv[i]);
		}
		if (temp->infile != NULL)
			printf("Infile = %s\n", temp->infile);
		if (temp->outfile != NULL)
			printf("Outfile = %s\n", temp->outfile);
		if (temp->errfile != NULL)
			printf("Errfile = %s\n", temp->errfile);
	}
}

void	free_command_list(void)
{
	int	i;

	CMD *temp1, *temp2;
	temp1 = command_list;
	while (temp1 != NULL)
	{
		free(temp1->name);
		for (i = 0; temp1->argv[i] != NULL; i++)
			free(temp1->argv[i]);
		if (temp1->infile != NULL)
			free(temp1->infile);
		if (temp1->infile != NULL)
			free(temp1->outfile);
		if (temp1->infile != NULL)
			free(temp1->errfile);
		temp2 = temp1;
		temp1 = temp2->next;
		free(temp2);
	}
}

struct command	*insert_command(void)
{
	CMD *new, *temp;
	new = (CMD *)malloc(sizeof(CMD));
	if (new == NULL)
	{
		perror("malloc error!\n");
		exit(1);
	}
	// É necessário inicializar o nodo, uma vez que não temos garantias do que a
	// função parse_line() o vai fazer (a linha de texto pode estar vazia...)
	new->name = NULL;
	new->argv[0] = NULL;
	new->infile = NULL;
	new->outfile = NULL;
	new->errfile = NULL;
	new->next = NULL;
	if (command_list == NULL)
	{ // É o primeiro nodo da lista?
		command_list = new;
	}
	else
	{ // Se não é o primeiro temos que ir para o fim da lista...
		for (temp = command_list; temp->next != NULL; temp = temp->next)
			;
		temp->next = new;
	}
	return (new); // retornamos o nodo vazio, mas inicializado, para ser
				// preenchido pela funcão parse_line()
}

void	parse_line(char *line)
{
	char	*a;
	int		count;
	CMD		*command;

	count = 0;
	char *name, *param, *infile, *outfile, *errfile;
	command_list = NULL;
	command = insert_command();
	for (a = strtok(line, " \t\r\n"); a != NULL; a = strtok(NULL, " \t\r\n"))
	{
		if (a[0] == '|')
		{
			command = insert_command();
			count = 0;
		}
		else if (a[0] == '<')
		{
			infile = a[1] ? a + 1 : strtok(NULL, " \t\r\n");
			command->infile = strdup(infile);
		}
		else if (a[0] == '>')
		{
			outfile = a[1] ? a + 1 : strtok(NULL, " \t\r\n");
			command->outfile = strdup(outfile);
		}
		else if (a[0] == '2' && a[1] == '>')
		{
			errfile = a[2] ? a + 2 : strtok(NULL, " \t\r\n");
			command->errfile = strdup(errfile);
		}
		else
		{
			if (!count)
			{
				name = a;
				command->name = strdup(name);
				command->argv[count] = strdup(name);
				count++;
			}
			else
			{
				param = a;
				if (count < MAXARGS - 1)
				{
					command->argv[count] = strdup(param);
					count++;
				}
			}
		}
	}
	command->argv[count] = NULL;
}
