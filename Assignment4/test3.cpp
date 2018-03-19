#include <bits/stdc++.h>
#include <MRFS.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

int main(){
  MRFS mrfs;
  mrfs.create_MRFS(10000000);
  mrfs.restore_myfs("memsys.backup");
  mrfs.ls_myfs();
  mrfs.showfile_myfs("mytest.txt");

  int fd = mrfs.open_myfs("mytest.txt", 'r');
  int fd1 = mrfs.open_myfs("sorted.txt", 'w');
  string s="";
  char ch[1];
  int bytes;
  while(!mrfs.eof_myfs(fd)){
    bytes = mrfs.read_myfs(fd, 1, ch);
    s += ch[0];
  }

  /*Convert strings to int and sort*/
  istringstream iss(s);
  std::vector<int> v;
  int t;
  while(!iss.eof()){
    iss >> t;
    v.push_back(t);
  }
  sort(v.begin(), v.end());
  string res = "";
  for(int i=0; i<v.size(); i++){
    res += to_string(v[i]) + "\n";
  }

  char res_array[res.size()+1];
  strcpy(res_array, res.c_str());
  mrfs.write_myfs(fd1, res.size(), res_array);
  mrfs.close_myfs(fd1);
  mrfs.rm_myfs(".filetable");

  mrfs.ls_myfs();
  mrfs.showfile_myfs("sorted.txt");
  return 0;
}
