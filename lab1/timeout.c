#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

void sigchld_handler(pid_t fork);

int main(int argc, char* argv[]){

    if(atoi(argv[1]) < 0){
        printf("%d is not a positive integer\n", atoi(argv[1]));
        exit(0);
    }
    if(argc < 3){
        printf("Usage: timeout sec command [args ...].\n");
        exit(0);
    }

    if(isdigit(*argv[1]) == 0){ //if second argument is non-numeric
        printf("%s is not a positive integer\n", argv[1]);
        exit(0);
    }
    

    pid_t forkReturn;
    forkReturn = fork(); //parent process creates a child process to run the command
    

    if(forkReturn < 0){
        perror("fork: ");
        exit(1);
    }

    else if(forkReturn == 0){
        //child process executes command here (second program that will eventully get terminated)
        
        char* envp[] = {NULL};

        char* args[argc-1];

        int counter = 0;
        for(counter; counter < argc - 2; counter++){
            args[counter] = (char*)malloc(sizeof(argv[counter+2]));
            strcpy(args[counter],argv[counter+2]);
        }
        args[argc-2] = '\0';
        
        execve(argv[2],args, envp); //execve() that runs the command the user enters "ls, ping, etc."
    }

    else{
        //this means we are in the parent process (the timeout program)
        
        signal(SIGCHLD, sigchld_handler);
        
        sleep(atoi(argv[1]));
        kill(forkReturn, SIGTERM);
    }
    
}

void sigchld_handler(pid_t fork){
    kill(fork, SIGTERM);
}