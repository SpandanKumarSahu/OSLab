#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <cstdlib>
#include <string> 
using namespace std;

int main(){
  string token, args_string;
  int pid, choice;
  bool flag = true;
  while(flag){
    //UI
    cout << "Press key corresponding to the task needed." <<endl;
    cout << "1. Run an internal command"<<endl;
    cout << "2. Run an external command"<<endl;
    cout << "3. Run an external command by redirecting standard input from a file"<<endl;
    cout << "4. Run an external command by redirecting standard output to a file"<<endl;
    cout << "5. Run an external command in the background"<<endl;
    cout << "6. Run several external commands in pipe mode"<<endl;
    cout << "7. Quit the shell." << endl;
    cin >> choice;
    getline(cin, token);

    switch(choice){
    case 1:
      {
	cin >> token;
	vector<string> args;
	string temp;
	while(cin.peek() != '\n'){
	  cin >> temp;
	  args.push_back(temp);
	}
	
	if(strcmp(token.c_str(), "chdir") == 0){
	  if (args.size() >= 1)
	    chdir(args[0].c_str());
	  else
	    cout<< "Please specify the directory to change to" <<endl;
	  return 0;
	} else if(strcmp(token.c_str(), "mkdir") == 0){
	  if (args.size() >= 1)
	    mkdir(args[0].c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	  else
	    cout<< "Please specify the directory to create" <<endl;
	}  else if(strcmp(token.c_str(), "rmdir") == 0){
	  if (args.size() >= 1)
	    rmdir(args[0].c_str());
	  else
	    cout<< "Please specify the directory to delete" <<endl;
	}
	break;
      }
    case 2:
      {
	// Input
	
	vector<string> args;
	string temp;
	while(cin.peek() != '\n'){
	  cin >> temp;
	  args.push_back(temp);
	}
	
	// Forking
	if ((pid = fork()) < 0)
	  perror("fork error");
	else if (pid == 0) {
	  char* arglist[args.size()+1];
	  for(int i = 0; i < args.size(); i++)
	    arglist[i] = strdup(args[i].c_str());
	  arglist[args.size()] = NULL;
	  if(args.size() > 0)
	    execvp(*arglist, arglist);
	  flag = false;
	  break;
	} else {
	  waitpid(pid, NULL, 0);
	  continue;
	}
	break;
      }
    case 3:
      {
	// Input
	vector<string> args;
	string temp;
	bool direction_op = false; 
	string file_name;
	while(cin.peek() != '\n'){
	  cin >> temp;
	  if(strcmp(temp.c_str(), "<") == 0){
	    direction_op = true;
	  } else if(direction_op == false){
	    args.push_back(temp);
	  } else{
	    file_name = temp;
	    break;
	  }
	}
	int fs = open(file_name.c_str(), O_RDONLY);
	if(fs < 0){
	  cout<<"Can't read file using file\n"<<endl;
	  break;
	}

	// Forking
	if ((pid = fork()) < 0)
	  perror("fork error");
	else if (pid == 0) {
	  char* arglist[args.size()+1];
	  for(int i = 0; i < args.size(); i++)
	    arglist[i] = strdup(args[i].c_str());
	  arglist[args.size()] = NULL;
	  close(0);
	  dup(fs);
	  if(args.size() > 0)
	    execvp(*arglist, arglist);
	  flag = false;
	  break;
	} else {
	  waitpid(pid, NULL, 0);
	  close(fs);
	}
	break;
      }
    case 4:
      {
	// Input
	vector<string> args;
	string temp;
	bool direction_op = false; 
	string file_name;
	while(cin >> temp){
	  if(strcmp(temp.c_str(), ">") == 0){
	    direction_op = true;
	  } else if(direction_op == true){
	    file_name = temp;
	    break;
	  } else{
	    args.push_back(temp);
	  }
	}
	int fs = open(file_name.c_str(), O_WRONLY | O_CREAT | O_SYNC, 0666);
	if(fs < 0){
	  cout<<"Can't read file using file\n"<<endl;
	  break;
	}

	// Forking
	if ((pid = fork()) < 0)
	  perror("fork error");
	else if (pid == 0) {
	  char* arglist[args.size()+1];
	  for(int i = 0; i < args.size(); i++)
	    arglist[i] = strdup(args[i].c_str());
	  arglist[args.size()] = NULL;
	  close(1);
	  dup(fs);
	  if(args.size() > 0)
	    execvp(*arglist, arglist);
	  flag = false;
	  break;
	} else {
	  waitpid(pid, NULL, 0);
	  close(fs);
	}
	break;
      }
    case 5:
      {
	// Input
	vector<string> args;
	string temp;
	while(cin.peek() != '\n'){
	  cin >> temp;
	  args.push_back(temp);
	}
	
	// Forking
	if ((pid = fork()) < 0)
	  perror("fork error");
	else if (pid == 0) {
	  char* arglist[args.size()+1];
	  for(int i = 0; i < args.size(); i++)
	    arglist[i] = strdup(args[i].c_str());
	  arglist[args.size()] = NULL;
	  if(args.size() > 0)
	    execvp(*arglist, arglist);
	  flag = false;
	  break;
	}
	break;
      }
    case 6: {
      string s;
      char dump;
      getline(cin,s);
      stringstream str(s);
      string token;
      int n=0;
      while(str >> token){
	n++;
      }

      char** arglist = new char*[n];
      stringstream str2(s);
      int i = 0;
      while(str2 >> token){
	char *arg = new char[token.size()+1];
	copy(token.begin(),token.end(),arg);
	arg[token.size()]='\0';
	arglist[i] = arg;
	i++;
      }
      
      int length = i;
      int commandnum = 0;
      for (i=0;i<length;i++){
	if (strcmp(arglist[i],"|") == 0)
	  {
	    commandnum++;
	  }
      }
      
      char** newarglist[commandnum+1];
      int k, j=0;
      i=0;
      while(i < length){
	k = i;
	int l = 0;
	while(strcmp(arglist[k],"|")!=0 && k < length){
	  k++;
	  l++;
	  if(k >= length) break;
	}
	newarglist[j] = new char*[l+1];
	
	k = i; l = 0;
	while(strcmp(arglist[k],"|")!=0 && k < length){
	  newarglist[j][l] = arglist[k];
	  k++;
	  l++;
	  if(k >= length) break;
	}
	newarglist[j][l] = 0;
	
	j++;
	k++;
	i = k;
      }
      
      int pipef[2];
      if( pipe(pipef) < 0 ) {
	perror("pipe");
	fflush(stderr);
      }
      /*
      for(int i = 0; i< commandnum; i++){
	int pid = fork();
	if(pid == 0 && i == 0){
	  close(STDOUT_FILENO);
	  dup(pipef[1]);
	  close(pipef[0]);
	  execvp(newarglist[i][0],newarglist[i]);
	}
	else if(pid == 0){
	    close(STDIN_FILENO);
	    dup(pipef[0]);
	    close(pipef[1]);
	    close(STDOUT_FILENO);
	    dup(pipef[1]);
	    close(pipef[0]);
	    execvp(newarglist[i][0],newarglist[i]);
	} else {
	  waitpid(pid, NULL, 0);
	}
      }
      int pid = fork();
      if(pid == 0){
	close(STDIN_FILENO);
	dup(pipef[0]);
	close(pipef[1]);
	execvp(newarglist[commandnum][0],newarglist[commandnum]);
      } else {
	waitpid(pid, NULL, 0);
	} */
      
      pid_t newp = fork();
      if(newp == 0){
	close(STDOUT_FILENO);
	dup(pipef[1]);
	close(pipef[0]);
	execvp(newarglist[0][0],newarglist[0]);
	flag = false;
	break;
      }
      else{
	pid_t newp2 = fork();
	if(newp2 == 0){
	  close(STDIN_FILENO);
	  dup(pipef[0]);
	  close(pipef[1]);
	  execvp(newarglist[1][0],newarglist[1]);
	  flag = false;
	  break;
	}
      }
      break;
    }
    case 7:{
      flag = false;
      break;
    }
    default:
      {
	flag = false;
	break;
      }
    }
  }
  return 0;
}
