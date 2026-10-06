#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

/*
 When a C program is executed, it is called as follows:
 
 int main (int argc, char *argv[], char *envp[]);
 
 where argc is the argument count, argv is an array of character pointers
 to the arguments themselves, and envp is an array of character pointers
 to the environment strings.  As indicated, argc is at least one, and the
 first member of the array points to a string containing the name of the
 file.
 
 */

int main(int argc, char **argv,char **envp)		// Command Line Arguments
{
    int i=0;
    pid_t child;
    
    child=fork();				// Fork system Call
    
    if(child==-1){
        perror("Fork");         // errno has the error code, perror generates the corresponding message
        exit(1);
    }
    
    if(child==0){				// Child Process
        for (i=0;i<100000;i++)
            printf("#");
    }
    else{					// Father Process
        for(i=0;i<100000;i++)
            printf("-");
    }
    
    exit(0);
}
