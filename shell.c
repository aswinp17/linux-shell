#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char **parse(char *line);
int run(char **args);
#define LENGTH 100
#define ARG 10

int main()
{

 char cmd[LENGTH];
 char **args;
 int cmdcount = 0,n;

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
   if(run(args) == 0) {
       break;
    }
 }
}


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **parse(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int run(char *args[])
{
    pid_t pid = fork();
    if (pid == -1) 
     {
      printf("error\n");
      return 1;
     }
    else if (pid == 0) 
     {
     
     execvp(args[0], args);  
     printf("error\n");
     return 0;
     }
} 



















































