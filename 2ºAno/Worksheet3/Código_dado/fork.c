
//processo filho: cópia do processo pai (código e dados) 
//processo pai e processo filho executam-se concorrentemente

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
  int pid; 
  if ((pid = fork()) == -1) {
    perror("fork");
    exit(1);
  } 
  if (pid == 0) { /* Processo filho */
    printf("Filho: PID do filho = %d, PID do pai = %d\n", getpid(), getppid());
    exit(0); 
  } 
  else { /* Processo pai */
    //sleep(1);
    printf("Pai: PID do filho = %d, PID do pai = %d\n", pid, getpid()); 
    exit(0);
  }
}
