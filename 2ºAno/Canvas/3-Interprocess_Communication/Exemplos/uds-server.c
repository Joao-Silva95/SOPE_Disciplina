#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 4096                                                       // block transfer size
#define LISTENQ 5                                                           // Size of the listen Queue


char *socket_path = "/tmp/socket";                                          // Unix domain socket name


int main(int argc, char *argv[])
{
    int listenfd,connfd,fd,bytes;
    char buf[BUF_SIZE];                                                     // buffer for outgoing file
    struct sockaddr_un channel_srv;
    
    if ( (listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {              // Creating the server socket
        perror("socket error");
        exit(-1);
    }
    
    unlink(socket_path);
    
    memset(&channel_srv, 0, sizeof(channel_srv));
    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path, socket_path, sizeof(channel_srv.sun_path)-1);
    
    if (bind(listenfd, (struct sockaddr*)&channel_srv, sizeof(channel_srv)) == -1) {      // Binding the server socket to its name
        perror("bind error");
        exit(-1);
    }
    if (listen(listenfd, LISTENQ) == -1) {                                  // Configuring the listen queue
        perror("listen error");
        exit(-1);
    }
    
                                                                            // Socket is now set up and bound. Waiting for connections
    while (1) {
        
        if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
        }
        
        if((bytes=read(connfd, buf, BUF_SIZE))<=0){
            close(connfd);
        }
        
                                                                            // Get and return the file.
        fd = open(buf, O_RDONLY);                                           // open the file to be sent back
        if (fd < 0) {
            perror("Open");
            close(connfd);
        }
        
        printf("Sending file: --> %s <-- back to client\n",buf);
        
        while (1) {
            bytes = read(fd, buf, BUF_SIZE);                                // read from file
            if (bytes <= 0) break;                                          // check for end of file
            write(connfd, buf, bytes);                                      // write bytes to socket
        }
        close(fd);                                                          // close file
        close(connfd);                                                      // close connection
    }
}
