#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(){
    kill(getppid(),SIGKILL);
}

//MATA SHELL