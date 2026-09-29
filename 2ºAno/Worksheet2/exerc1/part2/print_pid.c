#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(void) {
printf("PID do processo atual: %d\n", (int) getpid());
printf("PID do processo pai (PPID): %d\n", (int) getppid());
return 0;
}