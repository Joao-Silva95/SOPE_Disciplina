#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int	main(void)
{
	pid_t pid;

	pid = fork();

	if (pid < 0)
	{
		printf("Fork Failed\n");
		exit(-1);
	}
	else if (pid == 0)
	{ /* filho */
		printf("Antes do exec:  PID = %d, PPID = %d\n", getpid(), getppid());
		fflush(stdout); /* ver nota abaixo */
		execl("./pid_ppid", "pid_ppid", NULL);
		perror("execl"); /* só chega aqui se falhar */
		exit(1);
	}
	else
	{ /* pai */
		waitpid(pid, NULL, 0);
		printf("Child Complete (PID do pai = %d)\n", getpid());
		exit(0);
	}
}