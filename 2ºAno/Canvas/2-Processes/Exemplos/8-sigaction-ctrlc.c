#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
   
void handler()
{
    char ch;
    printf ("Do you really want to end the program (y|n)?\n");
    ch = getchar();
    if (ch == 'y') exit(0);
    else
            printf ("Ok, lets continue...\n");
}

int main ()
{
    struct sigaction sa;
    sigset_t block_mask;
    
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &handler;
    //sa.sa_flags = SA_RESETHAND;
    //If this bit is set, the handler is reset back to SIG_DFL at the moment the signal is delivered
    
    sigaddset (&block_mask, SIGINT);
    //add SIGINT to the signal block mask
    sa.sa_mask = block_mask;
    //During the execution of the handler reception of the SIGINT signal is blocked
    
    sigaction (SIGINT, &sa, NULL);
    
    
    printf("Handler associated with signal SIGINT\n");
    for (;;)
        sleep (10);
}
