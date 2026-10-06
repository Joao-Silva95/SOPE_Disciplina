//processo filho: cópia do processo pai (código e dados) 
//processo pai e processo filho executam-se concorrentemente

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
  int pid; 

  if ((pid = fork()) == -1) {
    perror("Fork");
    exit(1);
  } 
  if (pid == 0) { 			
    // Processo filho pid = 0
    printf("Filho: PID do filho = %d, PID do pai = %d\n", getpid(), getppid());
    exit(0); 
  } 
  else { 					
    // Processo pai pid = id filho
    printf("Pai: PID do filho = %d, PID do pai = %d\n", pid, getpid());
    wait(NULL);
    exit(0);
  }
}
