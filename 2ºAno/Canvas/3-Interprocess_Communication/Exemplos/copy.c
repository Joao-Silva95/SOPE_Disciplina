#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int	main(int argc, char *argv[])
{
	char	buffer[1024];

	int source, destination, n;
	source = open(argv[1], O_RDONLY);
	if (source == -1)
	{
		perror("Opening Source File");
		exit(1);
	}
	destination = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (destination == -1)
	{
		perror("Opening Destination File");
		exit(1);
	}
	while ((n = read(source, buffer, sizeof(buffer))) > 0)
		write(destination, buffer, n);
	exit(0);
}
