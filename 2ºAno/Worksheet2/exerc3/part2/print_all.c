#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(void) {
printf("PID: %d\n", (int) getpid());
printf("PPID: %d\n", (int) getppid());
printf("UID: %d\n", (int) getuid());
printf("EUID: %d\n", (int) geteuid());
printf("GID: %d\n", (int) getgid());
printf("EGID: %d\n", (int) getegid());
return 0;
}
