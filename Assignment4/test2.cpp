#include <bits/stdc++.h>
#include <MRFS.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

int main(){
  MRFS mrfs;
  int n;
  char ch[1];
  srand (time(NULL));
  mrfs.create_MRFS(10000000);
  int fd = mrfs.open_myfs("mytest.txt", 'w');
  int bytes = 0;
  for(int i=0; i<100; i++){
    n = rand()%10000 + 1;
    while(n!=0){
      ch[0] = '0' + n%10;
      n = n/10;
      bytes = mrfs.write_myfs(fd, 1, ch);
    }
    ch[0] = '\n';
    bytes = mrfs.write_myfs(fd, 1, ch);
  }
  cout << "Enter n: ";
  cin >> n;

  int fd0 = mrfs.open_myfs("mytest.txt", 'r');
  int fd_ar[n];
  for(int i=0; i<n; i++){
    fd_ar[i] = mrfs.open_myfs("mytest-"+to_string(i)+".txt", 'w');
  }

  mrfs.close_myfs(fd);
  while(!mrfs.eof_myfs(fd0)){
    bytes = mrfs.read_myfs(fd0, 1, ch);
    for(int i=0; i<n; i++)
      bytes = mrfs.write_myfs(fd_ar[i], 1, ch);
  }
  mrfs.close_myfs(fd0);
  for(int i=0; i<n; i++)
    mrfs.close_myfs(fd_ar[i]);
  mrfs.rm_myfs(".filetable");
  mrfs.dump_myfs("memsys.backup");
  return 0;
}
