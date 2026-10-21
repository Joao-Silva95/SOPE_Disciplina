#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 256
#define MAX_ARGS 64

int	main(void)
{
	char line[MAX_LINE];
	char *argv[MAX_ARGS];
	pid_t pid;

	while (1)
	{
		printf("shell> ");
		fflush(stdout);

		if (fgets(line, MAX_LINE, stdin) == NULL)
		{ /* Ctrl+D */
			printf("\n");
			break ;
		}

		/* remove o '\n' no fim */
		line[strcspn(line, "\n")] = '\0';

		/* separa a linha em palavras */
		int argc = 0;
		char *token = strtok(line, " \t");
		while (token != NULL && argc < MAX_ARGS - 1)
		{
			argv[argc++] = token;
			token = strtok(NULL, " \t");
		}
		argv[argc] = NULL; /* o vetor tem de terminar em NULL */

		/* linha vazia */
		if (argc == 0)
			continue ;

		/* comando interno */
		if (strcmp(argv[0], "exit") == 0)
			break ;

		pid = fork();

		if (pid < 0)
		{
			perror("fork");
			continue ;
		}
		else if (pid == 0)
		{ /* filho */
			execvp(argv[0], argv);
			perror(argv[0]); /* só chega aqui se falhar */
			exit(1);
		}
		else
		{ /* pai */
			waitpid(pid, NULL, 0);
		}
	}

	return (0);
}