#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_FILHOS 5

int	main(int argc, char *argv[])
{
	pid_t pids[N_FILHOS];
	int status;
	int i;

	/* O pai cria os 5 filhos */
	for (i = 0; i < N_FILHOS; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(1);
		}
		if (pids[i] == 0)
		{ /* Processo filho */
			srandom(getpid());
			int x = random() % 101; /* 0 a 100 */
			printf("Filho %d: PID = %d, valor = %d\n", i, getpid(), x);
			exit(x);
		}
		/* O pai NAO faz exit aqui: volta ao ciclo e cria o proximo filho */
	}

	/* O pai espera pelos filhos pela ordem em que foram criados */
	for (i = 0; i < N_FILHOS; i++)
	{
		if (waitpid(pids[i], &status, 0) == -1)
		{
			perror("waitpid");
			exit(1);
		}
		if (WIFEXITED(status))
		{
			printf("Pai: filho com PID %d devolveu %d\n", pids[i],
				WEXITSTATUS(status));
		}
	}

	return (0);
}