
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 0

#define MAX_LINE		80 /* 80 chars per line, per command */

typedef struct BGnode {
    int backgroundJob;
    int count;
    struct BGnode * next;
} BGProc;


typedef struct historyNode {
    char *line;
    int count;
    struct historyNode * next;
} historyList;

void
executeLine( char* line);

void
initScript( void );

void 
checkBGProcess(BGProc * head);

void
AddToList(int pid, int );

char*
findHistCommand(int i);

void 
historyPush(historyList * head, int count, char* line);

void
freeHistory(historyList * head);

//GLOBALS

BGProc * BGProcHead = NULL;

historyList * HistoryHead=NULL;

int verbose = 0;
int should_run = 1;

int historyCounter = 0;

int 
main(void)
{
    char line[MAX_LINE];

    initScript();
		
    while (should_run){   
        printf("bombshell>");
        fflush(stdout);
      	fgets(line, MAX_LINE, stdin);
        executeLine(line);
        if(verbose)
            fprintf(stderr ,"shouldrun: %d\n", should_run);
    }

    freeHistory(HistoryHead);
	return 0;
}


void
executeLine( char* line){

    char *args[MAX_LINE/2 + 1]; /* command line (of 80) has max of 40 arguments */
    
    char *token;

    // MIGHT NOT WANT TO STRIP QUOTES AND SHIT, grep "THING THING" =! grep THING THING
    char deliminiters[] = "  \n \t";
    //char deliminiters[] = "  \n \t \" \'";
    
    int i = 0;
    int upper;

    char temp[MAX_LINE];

    //printf("testing\n" );
    strcpy(temp, line);
    //printf("%s\n", temp);
     //stores first word in first buffer
        args[i] = strtok(line, deliminiters);
        i++;


        //loops through line till null terminator, and tokenizes each word
        while( args[i-1] != NULL){
            args[i] = strtok(NULL, deliminiters);
            i++;
        }

        //sets last args to null
        args[i] = NULL;
        upper = i - 1;
        pid_t pid;
        int background = 0;

        //debug prints out full parsed buffer
        if(DEBUG){
            for(i=0; i < upper; i++){
                printf("%s", args[i]);
                if(i != upper-1)
                    printf(" ");
            }
        }

        // Checks if buffer is empty
        if(args[0] == NULL){
            return;
        }

        //Checks if the current program should run in the background
        if(!strcmp(args[upper-1], "&")){
            background = 1;
            args[upper-1] = NULL;
            if(verbose){
                fprintf(stderr ,"Running the command in background.\n");
            }

            BGProc * newJob = malloc(sizeof(BGProc));

        }

        if(verbose){
                 fprintf(stderr ,"Number of arguments: %d.\n", upper);
        }

        //Prints out the linked list containing the past history
        if(!strcmp(args[0], "history")){
                historyList * ptr = HistoryHead;
                while (ptr != NULL) {
                    printf("%d %s\n", ptr->count, ptr->line);
                    ptr = ptr->next;
                }
                if(HistoryHead==NULL){
                    printf("There is no history\n");
                }
                return;

            }

            for(i=historyCounter-10; i < historyCounter; i++){
                if(i>-1){
                    char histCommand[MAX_LINE];
                    sprintf(histCommand, "%d!", historyCounter);
                    if(!strcmp(args[0], histCommand)){
                        executeLine(findHistCommand(i));
                        return;
                    }
                }
            }

        //Checks if it is a shell command
        // expand into function
            if(!strcmp(args[0], "exit")){
                should_run = 0;
                should_run = 0;
                should_run = 0;
                should_run = 0;
                if(verbose)
                fprintf(stderr ,"shouldrun: %d\n", should_run);
                return;
            }

            //Check for "!!", execture last item in history
            else if(!strcmp(args[0], "!!")){
                if(historyCounter < 10){
                    executeLine(findHistCommand(historyCounter));
                    return;
                }
            }

            //Checks if the first command is set
            else if(!strcmp(args[0], "set")){

                //checks if second command is verbose
                if(!strcmp(args[1], "verbose")){

                    //Sets verbose to on or off if it has "on" or "off" for third command
                    if(!strcmp(args[2], "on")){
                        verbose = 1;
                        return;
                    }
                    else if(!strcmp(args[2], "off")){
                        verbose = 0;
                        return;
                    }
                }
                //Checks if second command is path
                else if(!strcmp(args[1], "path")){

                    char *newPath;
                    strtok(temp, "()");
                    newPath = strtok(NULL, "()");
                    strcat(newPath, "\0");

                    i = 0;
                    while(newPath[i] != '\0'){
                        if(newPath[i] == ' '){
                            newPath[i] = ':';
                        }
                        i++;
                    }

                    setenv("PATH", newPath, 1);

                    if(verbose){
                        fprintf(stderr ,"PATH : %s\n", getenv("PATH"));
                    }
                    return;
                }
            }

            //Pipe Stdout to file and screen
            else if(!strcmp(args[0], "script")){
                // int fds[2];
                // pipe(fds);
                // dup2(fds[1], stdout);
                // read(fds[0], buf, buf_sz);
            }
            else if(!strcmp(args[0], "endscript")){
                //CLOSE PIPE
            }
        
            //Prints out if command is not a built in command
            else{
                if(verbose){
                    fprintf(stderr ,"Command is not a built in command.\n");
                }        
            }

        //strcpy( history[historyCounter], line);
       // historyPush(HistoryHead, historyCounter, line);

        //Adds the current command to the history
        if(HistoryHead == NULL){
            HistoryHead = (historyList *)malloc(sizeof(historyList));
            HistoryHead->next = NULL;
            HistoryHead->line = line;
            HistoryHead->count = historyCounter;
            printf("made head\n");
        }
        else{
            historyList * current = HistoryHead;
            while (current->next != NULL) {
                //printf("count:%d\n",current->count);
                current = current->next;
            }
            /* now we can add a new variable */
            current->next = (historyList *) malloc(sizeof(historyList));
            current->next->count = historyCounter;
            current->next->line = line;
            current->next->next = NULL;
        }


        historyCounter++;

        pid = fork();

        if(pid == 0){
            execvp(args[0], args);
            printf("failed to efind command: %s\n", line);
            fflush(stdout);                                                                                 
            exit(1);
        }
        else{
            if(!background){
                int returnStatus;    
                waitpid(pid, &returnStatus, 0); 
            }
            else{
                //Let child die
            }
        }
}


void
initScript(void){
    char line[MAX_LINE];

    char *home = getenv("HOME");
    printf("%s\n", home);

    char *initScript;
    initScript = strcat(home, "/.cs543rc");
   // printf("s%ss\n", initScript);

    //FILE *file = fopen(strcat(home, ".cs543rc"), "r");
    FILE *file = fopen(initScript, "r");

    if(file == NULL) 
    {
      perror("Error opening file");
      //return(-1);
    }

    while(fgets(line, MAX_LINE, file)) {
        printf("%s", line);
        executeLine(line);
    }

    if(verbose){
        fprintf(stderr ,"PATH : %s\n", getenv("PATH"));
        fprintf(stderr ,"HOME : %s\n", getenv("HOME"));
    }
}





void checkBGProcess(BGProc * head) {
    BGProc * current = BGProcHead;

    while (current != NULL) {
        printf("%d\n", current->backgroundJob);
        current = current->next;
    }
}

char*
findHistCommand(int i){
    historyList * current = HistoryHead;

    while(current!=NULL && current->count !=i){
        current = current->next;
    }

    if(current == NULL){
        return "Failed to find";
    }
    else
        return current->line;
}

void 
freeHistory(historyList * head){
    historyList * temp=NULL;
    while(head){
        temp=head->next;
    free(head);
    head=temp;
    }
}

void historyPush(historyList * head, int count, char* line) {
    
    // if(head == NULL){
    //     head = (historyList *)malloc(sizeof(historyList));
    //     head->next = NULL;
    //     head->line = line;
    //     head->count = count;
    //     printf("made head\n");
    // }
    // else{

    //     historyList * current = head;
    //     while (current->next != NULL) {
    //         printf("count:%d\n",current->count);
    //         current = current->next;
    //     }
    //     /* now we can add a new variable */
    //     current->next = (historyList *) malloc(sizeof(historyList));
    //     current->next->count = count;
    //     current->next->line = line;
    //     current->next->next = NULL;
    // }
}
