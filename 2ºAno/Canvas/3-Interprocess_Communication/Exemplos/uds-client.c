#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *socket_path = "/tmp/socket";

#define BUF_SIZE 4096			/* block transfer size */

int main(int argc, char **argv)
{
    int c, uds, bytes;
    char buf[BUF_SIZE];			/* buffer for incoming file */
    struct sockaddr_un channel;		/* Unix Domain socket */
    
    if (argc != 2) {
        printf("Usage: client file-name\n");
        exit(1);
    }
    
    if ( (uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    
    memset(&channel, 0, sizeof(channel));
    channel.sun_family= AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path)-1);
    
    if (connect(uds, (struct sockaddr*)&channel, sizeof(channel)) == -1) {
        perror("connect error");
        exit(1);
    }
    /* Connection is now established. Send file name including 0 byte at end. */
    write(uds, argv[1], strlen(argv[1])+1);
    
    /* Go get the file and write it to standard output. */
    
    while (1) {
        bytes = read(uds, buf, BUF_SIZE);	/* read from socket */
        if (bytes <= 0) exit(0);	/* check for end of file */
        write(1, buf, bytes);		/* write to standard output */
    }
}
