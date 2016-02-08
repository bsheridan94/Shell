
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int main(void)
{
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;

    char line[MAX_LINE];
    char *token;

    // MIGHT NOT WANT TO STRIP QUOTES AND SHIT, grep "THING THING" =! grep THING THING
    char deliminiters[] = "  \n \t \" \'";
	
	int i, upper;
		
    while (should_run){   
    	pid_t pid;

        printf("osh>");
        fflush(stdout);

        i = 0;
      	fgets(line, MAX_LINE, stdin);

        args[i] = strtok(line, deliminiters);
        i++;

        while( args[i-1] != NULL){
        	args[i] = strtok(NULL, deliminiters);
        	i++;
        }

        args[i] = NULL;

        upper = i - 1;

        for(i=0; i < upper; i++){
        	printf("%s", args[i]);
        	if(i != upper-1)
        		printf(" ");
        }

        pid = fork();

        if(pid == 0){
        	execvp(args[0], args);
        }
        

        printf("\n");
        
        /**
         * After reading user input, the steps are:
         * (1) fork a child process
         * (2) the child process will invoke execvp()
         * (3) if command included &, parent will invoke wait()
         */
    }


    
	return 0;
}
