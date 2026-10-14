#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int	main(int argc, char *argv[])
{
	int fd;
	pid_t pid;

	fd = open("teste", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	pid = fork();
	if (pid == 0)
	{
		close(1);
		dup(fd);
		close(fd);
		execlp("ls", "ls", NULL);
		perror("execlp");
		exit(1);
	}
	else
	{
		close(fd);
		wait(NULL);
	}
	return (0);
}