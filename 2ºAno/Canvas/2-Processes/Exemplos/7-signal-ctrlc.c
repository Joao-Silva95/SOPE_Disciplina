#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
   
void handler()
{
    char ch;
    printf ("Do you really want to end the program (y|n)?\n");
    ch = getchar();
    if (ch == 'y') {
        exit(0);
    }else{
        printf ("Ok, lets continue...\n");
    }
}

int main ()
{
    signal (SIGINT, handler);
    printf("Handler associated with signal SIGINT\n");
    for (;;)
        sleep (10);
}
