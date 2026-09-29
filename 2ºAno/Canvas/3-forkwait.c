/*
  The WIFEXITED macro is always successful.

  If the child process for which status was returned by the wait or waitpid function exited normally, the WIFEXITED macro evaluates to TRUE and the WEXITSTATUS macro is used to query the exit code of the child process. Otherwise, the WIFEXITED macro evaluates to FALSE.

  The WEXITSTATUS macro is always successful.

  If the WIFEXITED macro indicates that the child process exited normally, the WEXITSTATUS macro returns the exit code specified by the child. If the WIFEXITED macro indicates that the child process did not exit normally, the value returned by the WEXITSTATUS macro has no meaning.
*/
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



int main(int argc, char *argv[])
{ 
  int pid;
  if ((pid = fork()) == -1) {
    printf("ERRO!\n");
    exit(1);
  } 
  if (pid == 0) {
    // Processo Filho
    int x; 
    srandom(time(NULL)); 
    x = random()%100; 
    printf("O filho gerou o numero: %d e vai envia-lo para o pai.\n", x); 
    // Retorna um numero aleatorio
    exit(x);

    /*  man 3 exit ...Both functions make the low-order eight bits of the status argument available to a parent process which has called a wait(2)-family function.*/
      
  } 
  else {
    // Processo Pai
    int result;
    // fica a aguardar que o filho acabe
    wait(&result);
    if (WIFEXITED(result)) { 
      printf("Valor recebido no processo pai: %d.\n", WEXITSTATUS(result));
    } 
  }
}


