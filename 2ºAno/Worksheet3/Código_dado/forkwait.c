
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int	main(int argc, char *argv[])
{
	int	pid;
		int x;
		int result;

	if ((pid = fork()) == -1)
	{
		perror("fork");
		exit(1);
	}
	if (pid == 0)
	{ /*filho */
		srandom(getpid());
		x = random() % 100;
		printf("O filho gerou o numero: %d e vai envia-lo para o pai.\n", x);
		exit(x);
	}
	else
	{ /* Processo pai */
		wait(&result);
		if (WIFEXITED(result))
		{
			printf("Valor recebido no processo pai: %d.\n",
				WEXITSTATUS(result));
		}
	}
}
