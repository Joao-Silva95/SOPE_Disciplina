#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(){
    kill(getpid(),SIGKILL);
}

//mata-se