#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main ()
{
  /* The argument list to pass to the "ls" command. this list should always end with null cause execvp should known when the list of commands end */
    
  char* arg_list[] = {
    "ls",     /* argv[0], the name of the program.  */
    "-al",
    "/",
    NULL      /* The argument list must end with a NULL.  */
  };
    
  char* env_list[] = {
    "User=pmsobral",
    NULL      /* The env list must end with a NULL.  */
  };
	
  pid_t child_pid;
	
  child_pid = fork ();
  if (child_pid == 0){
    
    // Now execute the program "ls" in the context of the new process rewriting the code zone
      
    execl("/bin/ls", "ls", 0); // Absolute path to the binary file; inline list parameters, NULL terminated
    execv("/bin/ls", arg_list); // Absolute path to the binary file; variable parameters in a NULL terminated string array
    execvp(arg_list[0], arg_list); // Using $PATH to find to the binary file, variable parameters in a NULL terminated string array
    execlp("ls", "-l", "/", NULL); // Using $PATH to the binary file, inline list parameters, NULL terminated
    execve("/bin/ls", arg_list, env_list); // Absolute path to the binary file; variable parameters and environment NULL terminated string array
   
      
    // The execvp function returns only if an error occurs.
    printf ("Child Says:\tan error occurred in execvp\n");
    exit(1);
  }
  else {
    // Father Wait for the child process ends
    wait(NULL);
    printf ("Father Says:\tdone with main program\n");
    return 0;
  }
}
