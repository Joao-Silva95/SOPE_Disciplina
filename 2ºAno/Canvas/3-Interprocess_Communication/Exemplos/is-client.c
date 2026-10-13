/* This page contains the client program. The following one contains the
 * server program. Once the server has been compiled and started, clients
 * anywhere on the Internet can send commands (file names) to the server.
 * The server responds by opening and returning the entire file requested.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define SERVER_PORT 10007		/* arbitrary, but client and server must agree */
#define BUF_SIZE 4096			/* block transfer size */


int main(int argc, char **argv)
{
    int c, s, bytes;
    char buf[BUF_SIZE];			/* buffer for incoming file */
    struct hostent *h;			/* info about server */
    struct sockaddr_in channel;		/* holds IP address */
    
    if (argc != 3) {
        printf("Usage: is-client server-name file-name\n");
        exit(-1);
    }

    h = gethostbyname(argv[1]);		/* look up host's IP address */
    if (!h) {
        perror("gethostbyname");
        exit(-1);
    }
    
    
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        perror("socket");
        exit(-1);
    }
    
    memset(&channel, 0, sizeof(channel));
    channel.sin_family= AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    channel.sin_port= htons(SERVER_PORT);
    c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
    if (c < 0) {
        perror("connect");
        exit(-1);
    }
    
    
    /* Connection is now established. Send file name including 0 byte at end. */
    write(s, argv[2], strlen(argv[2])+1);
    
    /* Go get the file and write it to standard output. */
    
    while (1) {
        bytes = read(s, buf, BUF_SIZE);	/* read from socket */
        if (bytes <= 0) exit(0);	/* check for end of file */
        write(1, buf, bytes);		/* write to standard output */
    }
}
