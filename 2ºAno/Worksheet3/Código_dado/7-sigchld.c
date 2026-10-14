
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void	handler(int signal_number)
{
	switch (signal_number)
	{
	case SIGCHLD:
		printf("One of my children is done...\n");
		break ;
	case SIGUSR1:
		printf("Received a SIGUSR1...\n");
		break ;
	default:
		printf("Received a %d signal...\n", signal_number);
		break ;
	}
}

int	main(void)
{
	struct sigaction	sa;
	sigset_t			block_mask;
	int					i;
	pid_t				pid;
	unsigned int		tts;

	for (i = 0; i < 2; i++)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{
			sleep(i + 1);
			kill(getppid(), SIGUSR1);
			exit(0);
		}
	}
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &handler;
	// sa.sa_flags = SA_RESETHAND;
	// If this bit is set, the handler is reset back to SIG_DFL at the moment
	// the signal is delivered
	sigaddset(&block_mask, SIGINT);
	// add SIGUSR1 to the signal block mask
	sa.sa_mask = block_mask;
	// During the execution of the handler reception of the SIGINT signal is
	// blocked
	sigaction(SIGCHLD, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	while ((tts = sleep(3)) != 0)
		sleep(tts);
	wait(NULL);
	wait(NULL);
	return (0);
}
