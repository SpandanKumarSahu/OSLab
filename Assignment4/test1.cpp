#include <bits/stdc++.h>
#include <MRFS.h>
using namespace std;

int main(){
  MRFS mrfs;

  mrfs.create_MRFS(10000000);
  cout << "$ ";
  string command, value1, value2;
  int value3;
  cin >> command;
  int x;
  while(command.compare("exit") != 0){
    if(command.compare("copy2myfs") == 0){
      cin >> value1 >> value2;
      x = mrfs.copy_pc2myfs(value1, value2);
    } else if(command.compare("copy2pc") == 0){
      cin >> value1 >> value2;
      x = mrfs.copy_myfs2pc(value1, value2);
    } else if(command.compare("rm") == 0){
      cin >> value1;
      x = mrfs.rm_myfs(value1);
    } else if(command.compare("cat") == 0){
      cin >> value1;
      x = mrfs.showfile_myfs(value1);
    } else if(command.compare("ls") == 0){
      x = mrfs.ls_myfs();
    } else if(command.compare("mkdir") == 0){
      cin >> value1;
      x = mrfs.mkdir_myfs(value1);
    } else if(command.compare("cd") == 0){
      cin >> value1;
      x = mrfs.chdir_myfs(value1);
    } else if(command.compare("rmdir") == 0){
      cin >> value1;
      x = mrfs.rmdir_myfs(value1);
    } else if(command.compare("dump") == 0){
      cin >> value1;
      x = mrfs.dump_myfs(value1);
    } else if(command.compare("restore") == 0){
      cin >> value1;
      x = mrfs.restore_myfs(value1);
    } else if(command.compare("status") == 0){
      x = mrfs.status_myfs(0);
    } else if(command.compare("status-details") == 0){
      x = mrfs.status_myfs(1);
    } else if(command.compare("status-details-inode") == 0){
      x = mrfs.status_myfs(1, 1);
    } else if(command.compare("chmod") == 0){
      cin >> value1 >> value3;
      x = mrfs.chmod_myfs(value1, value3);
    } else{
      cout << "Invalid terminal based commands." << endl;
    }
    cout << "$ ";
    cin >> command;
  }
  return 0;
}
