#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int	main(void)
{
	printf("PID do processo atual: %d\n", (int)getpid());
	printf("PID do processo pai (PPID): %d\n", (int)getppid());
	return (0);
}