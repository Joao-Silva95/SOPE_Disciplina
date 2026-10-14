#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NP1 "/tmp/np1"
#define NP2 "/tmp/np2"
#define MAX_BUF_SIZE 255

int	main(int argc, char *argv[])
{
	int wrfd, rdfd, numread;
	char rdbuf[MAX_BUF_SIZE];

	/* Check if an argument was specified. */

	if (argc != 2)
	{
		printf("Usage : %s <string to be sent to the server>n", argv[0]);
		exit(1);
	}

	/* Open the first named pipe for writing */
	wrfd = open(NP1, O_WRONLY);

	/* Open the second named pipe for reading */
	rdfd = open(NP2, O_RDONLY);

	/* Write to the pipe */
	write(wrfd, argv[1], strlen(argv[1]));

	/* Read from the pipe */
	numread = read(rdfd, rdbuf, MAX_BUF_SIZE);

	rdbuf[numread] = '\0';

	printf("Full Duplex Client : Read From the Pipe : %s\n", rdbuf);
}