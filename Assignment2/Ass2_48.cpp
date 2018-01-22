#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <bits/stdc++.h>
#include <string>
#include <iostream>
using namespace std;

int main(){
  string token, args_string;
  int pid, choice;
  bool flag = true;
  while(flag){
    //UI
    cout << "Press key corresponding to the task needed." <<endl;
    cout << "1. Run an external command"<<endl;
    cout << "2. Run an internal command"<<endl;
    cout << "3. Run an external command by redirecting standard input from a file"<<endl;
    cout << "4. Run an external command by redirecting standard output to a file"<<endl;
    cout << "5. Run an external command in the background"<<endl;
    cout << "6. Run several external commands in pipe mode"<<endl;
    cout << "7. Quit the shell." << endl;
    cin >> choice;

    switch(choice){
    case 1:
      if(strcmp(token.c_str(), "chdir") == 0){
	if (args.size() > 1)
	  chdir(args[0].c_str());
	else
	  cout<< "Please specify the directory to change to" <<endl;
	return 0;
      } else if(strcpm(toke.c_str(), "mkdir") == 0){
	if (args.size() > 1)
	  mkdir(args[0].c_str());
	else
	  cout<< "Please specify the directory to create" <<endl;
	return 0;
      }  else if(strcpm(toke.c_str(), "rmdir") == 0){
	if (args.size() > 1)
	  rmdir(args[0].c_str());
	else
	  cout<< "Please specify the directory to delete" <<endl;
	return 0;
      }
      break;
    case 2:
      {
	// Input
	cin >> token;
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
	  if(args.size() > 0){
	    execvp(token.c_str(), arglist);
	  }
	  else{
	    execlp(token.c_str(), token.c_str(), NULL);
	  }
	  flag = false;
	  break;
	} else {
	  wait(&res);
	}
	break;
      }
    case 3:
      {
	close(0);
	int fs = open ("myfile.txt", O_WRONLY | O_CREAT);
	if(fs < 0){
	  cout<<"Can¡¯t read file using file\n"<<endl;
	  exit (1);
	}
	dup(fs);

	// Input
	cin >> token;
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
	  if(args.size() > 0){
	    execvp(token.c_str(), arglist);
	  }
	  else{
	    execlp(token.c_str(), token.c_str(), NULL);
	  }
	  flag = false;
	  break;
	} else {
	  wait(&res);
	}
	close(fs);
	dup(0);
	break;
      }
    case 4:
      {
	close(1);
	int fs = open ("myfile.txt", O_WRONLY | O_CREAT);
	if(fs < 0){
	  cout << "Can¡¯t read file using file\n"<< endl;
	  exit (1);
	}
	dup(fs);

	// Input
	cin >> token;
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
	  if(args.size() > 0){
	    execvp(token.c_str(), arglist);
	  }
	  else{
	    execlp(token.c_str(), token.c_str(), NULL);
	  }
	  flag = false;
	  break;
	} else {
	  wait(&res);
	}
	close(fs);
	dup(1);
	break;
      }
    case 5:
      {
	// Input
	cin >> token;
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
	  if(args.size() > 0){
	    execvp(token.c_str(), arglist);
	  }
	  else{
	    execlp(token.c_str(), token.c_str(), NULL);
	  }
	  flag = false;
	  break;
	}
	break;
      }
    case 6:
      {
	bool sub_cmd = true;
	int count = 0;
	int mypipe1[2], mypipe2[2];
	while(sub_cmd){
	  // Input
	  cin >> token;
	  vector<string> args;
	  string temp;
	  
	  while(cin.peek() != '\n'){
	    cin >> temp;
	    args.push_back(temp);
	    if(strcmp(args[i].c_str(),"|") == 0){
	      if(count == 0){
		if (pipe(mypipe1) || pipe(mypipe2)){
		  fprintf(stderr, "Pipe failed.\n");
		  return EXIT_FAILURE;
		}
		close(1);
		dup(mypipe1[1]);
		close(mypipe1[0]);

		// Forking
		if ((pid = fork()) < 0)
		  perror("fork error");
		else if (pid == 0) {
		  char* arglist[args.size()+1];
		  for(int i = 0; i < args.size(); i++)
		    arglist[i] = strdup(args[i].c_str());
		  arglist[args.size()] = NULL;
		  if(args.size() > 0){
		    execvp(token.c_str(), arglist);
		  }
		  else{
		    execlp(token.c_str(), token.c_str(), NULL);
		  }
		  flag = false;
		  break;
		} else {
		  wait(&res);
		}
		close(mypipe[1]);
		dup(mypipe[0]);
		close(0);
		dup(mypipe[1]);
	      } else if(i%2==1){
		close(mypipe[1]);
		dup(mypipe[0]);
		close(mypipe[0]);
		dup(mypipe[1]);
	      }
	      count++;
	      break;
	    }
	  }
	  if(cin.peek() == '\n'){
	    sub_cmd = false;
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
