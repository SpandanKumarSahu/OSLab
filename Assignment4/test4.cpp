#include <bits/stdc++.h>
#include <MRFS.h>
using namespace std;

int main(){
  MRFS mrfs;

  mrfs.create_MRFS(10000000);
  mrfs.mkdir_myfs("myroot");
  mrfs.chdir_myfs("myroot");

  mrfs.mkdir_myfs("mydocs");
  mrfs.chdir_myfs("mydocs");

  mrfs.mkdir_myfs("mytext");
  mrfs.mkdir_myfs("mypapers");
  mrfs.chdir_myfs("..");

  mrfs.mkdir_myfs("mycode");
  mrfs.chdir_myfs("..");

  mrfs.ls_myfs();
  srand(time(NULL));

  int pid = fork();
  if(pid == 0){
    mrfs.chdir_myfs("myroot");
    mrfs.chdir_myfs("mydocs");
    mrfs.chdir_myfs("mytext");
    int fd = mrfs.open_myfs("P1.txt", 'w');
    char ar[26];
    for(int i=0; i<26; i++)
      ar[i] ='A'+i;
    mrfs.write_myfs(fd, 26, ar);
    mrfs.close_myfs(fd);
    sleep(rand()%2);
    cout << "CHILD PROCESS" << endl;
    mrfs.ls_myfs();
    mrfs.showfile_myfs("P1.txt");
  } else {
    mrfs.chdir_myfs("myroot");
    mrfs.chdir_myfs("mycode");
    mrfs.copy_pc2myfs("myfile.txt", "file1");
    mrfs.copy_pc2myfs("smallfile.txt", "file2");
    sleep(3 + rand()%2);
    cout << "\n\nPARENT PROCESS" << endl;
    mrfs.ls_myfs();
    mrfs.showfile_myfs("P1.txt");
  }
  return 0;
}
