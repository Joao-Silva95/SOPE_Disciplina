#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int main(int argc, char *argv[])
{
    char cwd[PATH_MAX];
    char cwd_novo[PATH_MAX];
    char esperado[PATH_MAX];
    const char *path = getenv("PATH");
    const char *home = getenv("HOME");
    const char *destino = (argc > 1) ? argv[1] : "/tmp";

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
        printf("CWD (antes)  : %s\n", cwd);
    }
    else
    {
        perror("Erro ao obter o diretorio atual (getcwd)");
        return EXIT_FAILURE;
    }

    printf("\nA mudar para: %s\n", destino);
    if (chdir(destino) != 0)
    {
        perror("Erro em chdir");
        return EXIT_FAILURE;
    }

    /* Verificar se getcwd() reflete a mudanca */
    if (getcwd(cwd_novo, sizeof(cwd_novo)) != NULL)
    {
        printf("CWD (depois) : %s\n", cwd_novo);
    }
    else
    {
        perror("Erro ao obter o diretorio atual apos chdir (getcwd)");
        return EXIT_FAILURE;
    }

    /* getcwd devolve o caminho real (sem symlinks), por isso comparamos
       com o realpath() do destino e nao com a string original */
    if (realpath(destino, esperado) == NULL)
    {
        perror("Erro em realpath");
        return EXIT_FAILURE;
    }

    if (strcmp(cwd_novo, esperado) == 0)
    {
        printf("Verificacao  : getcwd() REFLETE a mudanca feita por chdir().\n");
    }
    else
    {
        printf("Verificacao  : getcwd() NAO corresponde ao destino!\n");
    }

    return EXIT_SUCCESS;
}