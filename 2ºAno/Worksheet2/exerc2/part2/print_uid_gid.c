#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(void) {
printf("PID do user atual: %d\n", (int) getuid());
printf("PID do user efetivo: %d\n", (int) geteuid());
printf("PID do group atual: %d\n", (int) getgid());
printf("PID do group efetivo: %d\n", (int) getegid());
return 0;
}