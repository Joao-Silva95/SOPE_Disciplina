/* sig_talk.c --- Example of how 2 processes can talk */
/* to each other using kill() and signal() */
/* We will fork() 2 process and let the parent send a few */
/* signals to it`s child  */

/* gcc 6-sig_talk.c -o 6-sig_talk  */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/* routines child will call upon sigtrap */
void sighup();
void sigint();
void sigquit();

int main()
{
    int pid;
    
    // get child process
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0)
        // Child
    {
        // set function calls
        signal(SIGHUP,sighup);
        signal(SIGINT,sigint);
        signal(SIGQUIT, sigquit);
        for(;;); // loop for ever
    }
    else
        // Parent
    {
        // pid hold id of child
        // pause for 3 secs
        sleep(3);
        printf("\nPARENT: sending SIGHUP\n\n");
        // send signal
        kill(pid,SIGHUP);
        // pause for 3 secs
        sleep(3);
        printf("\nPARENT: sending SIGINT\n\n");
        // send signal
        kill(pid,SIGINT);
        // pause for 3 secs
        sleep(3);
        printf("\nPARENT: sending SIGQUIT\n\n");
        // send signal
        kill(pid,SIGQUIT);
        // pause for 3 secs
        sleep(3);
        wait(NULL);
    }
}

void sighup()

{
    printf("CHILD: I have received a SIGHUP\n");
}

void sigint()

{
    printf("CHILD: I have received a SIGINT\n");
}

void sigquit()

{ 
    printf("My DADDY told me to DIE NOW!!!\n\n\n");
    exit(0);
}

