#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

char **parse(char *line);
int commandHandler(char * args[]);
#define LENGTH 100
#define ARG 10
const int PIPE_READ = 0;
const int PIPE_WRITE = 1;
pid_t pid;

int main()
{

 char cmd[LENGTH];
 char **args;


 while(1){
   
   printf(">>");
   if(fgets(cmd,sizeof(cmd),stdin) == NULL){
      break;
    }
   
  if(cmd[strlen(cmd)-1] == '\n') {
       cmd[strlen(cmd)-1] == '\0';
    }
   
   args = parse(cmd);
   
   if(strcmp(args[0],"exit") == 0){
      break;
   }
   if(commandHandler(args) == 0) 
      break;
      
   
    bzero(args, sizeof(args)); 
    
    fflush(stdout);    
    
 }
}




#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
char **parse(char *line)
{
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, " allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    

    token = strtok(NULL, TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void launchProg(char **args, int background){	 
	 int err = -1;
	 
	 if((pid=fork())==-1){
		 printf("Child process could not be created\n");
		 return;
	 }
	
	if(pid==0){
		
		if (execvp(args[0],args)==err){
			printf("Command not found");
			kill(getpid(),SIGTERM);
		}
	 }
	 
	 
	 if (background == 0){
		 waitpid(pid,NULL,0);
	 }else{
		 
		 printf("Process created with PID: %d\n",pid);
	 }	 
}
 

void fileIO(char * args[], char* inputFile, char* outputFile, int option){
	 
	int err = -1;
	
	int fileDescriptor; 
	
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){
		
		if (option == 0){
		
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
		        dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		
		}else if (option == 1){
			
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);		 
		}
		 
		
		
		
	}
	waitpid(pid,NULL,0);
}


 
void pipeHandler(char * args[]){
	
        int filedes[2]; 
	int filedes2[2];
	
	int num_cmds = 0;
	
	char *command[256];
	
	pid_t pid;
	
	int err = -1;
	int end = 0;
	
	
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	
	
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;
	
	
	while (args[j] != NULL && end != 1){
		k = 0;
		
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){
				
				end = 1;
				k++;
				break;
			}
			k++;
		}
		
		command[k] = NULL;
		j++;		
		
		
		if (i % 2 != 0){
			pipe(filedes); 
		}else{
			pipe(filedes2);
		}
		
		pid=fork();
		
		if(pid==-1){			
			if (i != num_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); 
				}else{
					close(filedes2[1]);
				} 
			}			
			printf("Child process could not be created\n");
			return;
		}
		if(pid==0){
			

			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){ 
					dup2(filedes[0],STDIN_FILENO);
				}else{ 
					dup2(filedes2[0],STDIN_FILENO);
				}
			
			}else{ 
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO); 
					dup2(filedes[1],STDOUT_FILENO);
				}else{ 
					dup2(filedes[0],STDIN_FILENO); 
					dup2(filedes2[1],STDOUT_FILENO);					
				} 
			}
			
			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}		
		}
				
		
		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1){
			if (num_cmds % 2 != 0){					
				close(filedes[0]);
			}else{					
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){					
				close(filedes2[0]);
				close(filedes[1]);
			}else{					
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}
			

int commandHandler(char * args[]){
	int i = 0;
	int j = 0;
	
	int fileDescriptor;
	int standardOut;
	
	int aux;
	int background = 0;
	
	char *args_aux[256];
	
	
	while ( args[j] != NULL){
		if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
			break;
		}
		args_aux[j] = args[j];
		j++;
	}
	
	// 'exit' command quits the shell
	if(strcmp(args[0],"exit") == 0) 
            exit(0);
	
        else{
		
		while (args[i] != NULL ){
			
			 if (strcmp(args[i],"|") == 0){
				pipeHandler(args);
				return 1;
			
			}else if (strcmp(args[i],"<") == 0){
				aux = i+1;
				if (args[aux] == NULL || args[aux+1] == NULL || args[aux+2] == NULL ){
					printf("Not enough input arguments\n");
					return -1;
				}else{
					if (strcmp(args[aux+1],">") != 0){
						printf("Usage: Expected '>' and found %s\n",args[aux+1]);
						return -2;
					}
				}
				fileIO(args_aux,args[i+1],args[i+3],1);
				return 1;
			}
			
			else if (strcmp(args[i],">") == 0){
				if (args[i+1] == NULL){
					printf("Not enough input arguments\n");
					return -1;
				}
				fileIO(args_aux,NULL,args[i+1],0);
				return 1;
			}
			i++;
		}
		
		args_aux[i] = NULL;
		launchProg(args_aux,background);
		
		
	}
return 1;
}















































