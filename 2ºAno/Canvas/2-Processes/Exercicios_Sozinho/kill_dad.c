#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int	main(void)
{
	kill(getppid(), SIGKILL);
}

// MATA SHELL