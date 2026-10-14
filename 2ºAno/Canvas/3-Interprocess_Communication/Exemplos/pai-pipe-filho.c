#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define TAMSG 100

char	msg[] = "mensagem de teste";
char	tmp[TAMSG];

int	main(void)
{
	int fds[2], pid_filho;
	if (pipe(fds) < 0)
		exit(-1);
	// Qual o valor dos descritores fds[0] e fds[1]?
	// printf("fds[0]=%d, fds[1]=%d\n",fds[0],fds[1]);
	if (fork() == 0)
	{ /* lê do pipe */
		read(fds[0], tmp, sizeof(msg));
		printf("fds[0]=%d, fds[1]=%d\n", fds[0], fds[1]);
		printf("%s\n", tmp);
		exit(0);
	}
	else
	{
		write(fds[1], msg, sizeof(msg));
		wait(NULL);
	}
}
