#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 256

int	main(void)
{
	char cmd[MAX];
	pid_t pid;

	while (1)
	{
		printf("shell> ");
		fflush(stdout);

		/* lê a linha; fgets devolve NULL em Ctrl+D (EOF) */
		if (fgets(cmd, MAX, stdin) == NULL)
		{
			printf("\n");
			break ;
		}

		/* remove o '\n' no fim */
		cmd[strcspn(cmd, "\n")] = '\0';

		/* linha vazia: mostra novo prompt */
		if (cmd[0] == '\0')
			continue ;

		/* comando interno para sair */
		if (strcmp(cmd, "exit") == 0)
			break ;

		pid = fork();

		if (pid < 0)
		{
			perror("fork");
			continue ;
		}
		else if (pid == 0)
		{ /* filho */
			execlp(cmd, cmd, NULL);
			perror(cmd); /* só chega aqui se falhar */
			exit(1);
		}
		else
		{ /* pai */
			waitpid(pid, NULL, 0);
		}
	}

	return (0);
}