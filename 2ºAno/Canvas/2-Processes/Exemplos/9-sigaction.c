
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


void handler (int signal_number)
{
    char ch;
    
    switch(signal_number){
        case SIGCHLD:
            printf ("SIGCHLD received!\n");
            break;
        case SIGINT:
            printf ("Do you really want to end the program (y|n)?\n");
            ch = getchar();
            if (ch == 'y') exit(0);
            else
                    printf ("Ok, lets continue...\n");
            break;
        case SIGUSR1:
            printf ("SIGUSR1 received!\n");
            break;
        default:
            printf ("Received a %d signal...\n",signal_number);
            break;
    }
}

int main ()
{
    int i, pid;
    struct sigaction sa;
    
    
    for(i=0;i<2;i++){
        pid=fork();
        if(pid<0){
            perror("fork");
            exit(1);
        }
        if(pid==0){
            sleep(i+2);
            kill(getppid(),SIGUSR1);
            exit(0);
        }
    }
    
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &handler;
    
    
    sigaction (SIGCHLD, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGUSR1, &sa, NULL);
    
    for(;;) sleep(10);
}
