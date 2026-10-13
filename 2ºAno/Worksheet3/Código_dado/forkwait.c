
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



int main(int argc, char *argv[])
{
    int pid;
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) { /*filho */
        int x;
		srandom(getpid());
        x = random()%100;
        printf("O filho gerou o numero: %d e vai envia-lo para o pai.\n", x);
        exit(x);
    }
    else {	/* Processo pai */
        int result;
        wait(&result);
        if (WIFEXITED(result)) {
            printf("Valor recebido no processo pai: %d.\n", WEXITSTATUS(result));
        } 
    }
}


