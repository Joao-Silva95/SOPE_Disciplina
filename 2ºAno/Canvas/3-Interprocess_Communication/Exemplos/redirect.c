#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define TAMSG 100 

char msg[] = "mensagem de teste";
char tmp[TAMSG];

int main(void) { 
	int fds[2], pid_filho;

	if (pipe (fds) < 0) exit(-1);

	if (fork () == 0) {
		/* liberta o stdin (posição zero) */
		close(0);
		/* redirecciona o stdin para o pipe de leitura */
		dup (fds[0]);
		/* fecha os descritores não usados pelo filho */
		close (fds[0]); 
		close (fds[1]);
		
		/* lê do pipe */ 
		read (0, tmp, sizeof (msg)); 
		printf ("%s\n", tmp); 
		exit (0);
	}
	else {
		/* liberta o stdout (posição um) */
		close(1);
		/* redirecciona o stdout para o pipe de escrita */
		dup (fds[1]);
		/* fecha os descritores não usados pelo pai */
		close (fds[0]); 
		close (fds[1]);
		write (1, msg, sizeof (msg)); 
		wait(NULL);
	}
}
