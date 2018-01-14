#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main(){
  char str[200];
  int pid;
  char *token, *args;
  bool flag = true;
  while(flag){
    scanf("%[^\n]%*c", str);
    token = strtok(str, " ");
    args = strtok(NULL, " ");
    for(int i = 0; token[i] != '\0'; i++)
      token[i] = tolower(token[i]);
    if(strcmp(token, "quit")==0)
      flag = false;
    else{
      if ((pid = fork()) < 0)
	perror("fork error");
      else if (pid == 0) {
	execlp(token, token, args, NULL);
	flag = false;
      }
    }
  }
  return 0;
}
