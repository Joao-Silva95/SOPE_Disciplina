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
#define QUEUE_SIZE 10


int main(int argc, char *argv[])
{
    int s, b, l, fd, sa, bytes, on = 1;
    char buf[BUF_SIZE];			/* buffer for outgoing file */
    struct sockaddr_in channel_srv;	/* hold's IP address */
    
    /* Build address structure to bind to socket. */
    memset(&channel_srv, 0, sizeof(channel_srv));	/* zero channel */
    channel_srv.sin_family = AF_INET;
    channel_srv.sin_addr.s_addr = htonl(INADDR_ANY);
    channel_srv.sin_port = htons(SERVER_PORT);
    
    /* Passive open. Wait for connection. */
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
    if (s < 0) {
        perror("socket error");
        exit(-1);
    }
    
    
    b = bind(s, (struct sockaddr *) &channel_srv, sizeof(channel_srv));
    if (b < 0) {
        perror("bind error");
        exit(-1);
    }
    l = listen(s, QUEUE_SIZE);		/* specify queue size */
    if (l < 0) {
        perror("listen error");
        exit(-1);
    }
    /* Socket is now set up and bound. Wait for connection and process it. */
    while (1) {
        sa = accept(s,0,0);		/* block for connection request */
        if (sa < 0) {
            perror("Accept");
            continue;
        }
        
        if((bytes=read(sa, buf, BUF_SIZE))<=0){
            close(sa);
        }
        /* Get and return the file. */
        fd = open(buf, O_RDONLY);	/* open the file to be sent back */
        if (fd < 0) {
            perror("Open");
            close(sa);
        }
        
        
        while (1) {
            bytes = read(fd, buf, BUF_SIZE);    /* read from file */
            if (bytes <= 0) break;              /* check for end of file */
            write(sa, buf, bytes);              /* write bytes to socket */
        }
        close(fd);				 /* close file */
        close(sa);				 /* close connection */
    }
}
