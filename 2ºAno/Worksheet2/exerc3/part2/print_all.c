#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int main(void)
{
    char cwd[PATH_MAX];
    const char *path = getenv("PATH");
    const char *home = getenv("HOME");

    printf("PID: %d\n", (int)getpid());
    printf("PPID: %d\n", (int)getppid());
    printf("UID: %d\n", (int)getuid());
    printf("EUID: %d\n", (int)geteuid());
    printf("GID: %d\n", (int)getgid());
    printf("EGID: %d\n", (int)getegid());

    printf("PATH : %s\n", path ? path : "(nao definida)");
    printf("HOME : %s\n", home ? home : "(nao definida)");

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("CWD  : %s\n", cwd);
    }
    else
    {
        perror("Erro ao obter o diretorio atual (getcwd)");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
