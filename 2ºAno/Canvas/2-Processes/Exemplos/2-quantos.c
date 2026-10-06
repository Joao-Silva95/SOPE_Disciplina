
//Quantos processo são criados ?

#include <stdio.h>
#include <unistd.h>

int main () { 
  // create a process
  fork() ; 
  fork() ; 
  fork() ; 
  // for each process that have been created before exited they should printf their PID
  printf("Processo %d\n", getpid() );
}
