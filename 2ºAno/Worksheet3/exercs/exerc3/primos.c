#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10

void	imprime_vetor(int v[])
{
	int	i;

	printf("primos = [");
	for (i = 0; i < N; i++)
	{
		printf("%d%s", v[i], i < N - 1 ? ", " : "");
	}
	printf("]\n");
}

/* Soma as posicoes com indice = inicio, inicio+2, ... e poe-nas a zero */
int	soma_e_zera(int v[], int inicio)
{
	int	soma;
	int	i;

	soma = 0;
	for (i = inicio; i < N; i += 2)
	{
		soma += v[i];
		v[i] = 0;
	}
	return (soma);
}

int	main(int argc, char *argv[])
{
	int primos[N] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
	pid_t pids[2];
	int status;
	int soma;
	int i;

	for (i = 0; i < 2; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(1);
		}
		if (pids[i] == 0)
		{ /* Filho 0: pares (0,2,4,6,8); Filho 1: impares */
			soma = soma_e_zera(primos, i);
			printf("Filho %d: PID = %d, soma = %d\n", i, getpid(), soma);
			printf("Filho %d: ", i);
			imprime_vetor(primos);
			exit(soma);
		}
	}

	for (i = 0; i < 2; i++)
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

	printf("Pai: ");
	imprime_vetor(primos);
	return (0);
}