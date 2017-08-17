#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int parse(char* cmd, char **params);
int run(char **args);
#define LENGTH 100
#define ARG 10

int main()
{

 char cmd[LENGTH];
 char *args[ARG];
 int cmdcount = 0,n;

 while(1){
   
   printf(">>");
   if(fgets(cmd,sizeof(cmd),stdin) == NULL){
      break;
    }
   if(cmd[strlen(cmd)-1] == '\n') {
       cmd[strlen(cmd)-1] == '\0';
    }
   int count = parse(cmd, args);
   if(strcmp(args[0],"exit") == 0){
      break;
   }
   if(run(args) == 0) {
       break;
    }
 }
}

int parse(char *cmd, char **args)
{
  int n = -1;
  for(int i = 0 ; i < ARG; i++) {
      args[i] = strsep(&cmd," ");
      n++;
      if(args[i] == NULL)
      break;
     }
  return (n) ;
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




















































