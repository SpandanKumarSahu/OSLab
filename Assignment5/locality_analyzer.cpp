#include "matplotlibcpp.h"
#include <bits/stdc++.h>

using namespace std;
namespace plt = matplotlibcpp;
std::vector<int> page_ref;

void analyze_locality_diff(){
  std::vector<int> v;
  for(int i=0; i<page_ref.size()-1; i++)
    v.push_back(abs(page_ref[i] - page_ref[i+1]));
  plt::plot(v);
  plt::show();
}

int main() {
  ifstream fs;
  fs.open("trace.txt");
  bool type;
  int page_num;
  while(!fs.eof()){
    fs >> type >> page_num;
    page_ref.push_back(page_num);
  }
  fs.close();

  // analyze_locality();
  analyze_locality_diff();
  return 0;
}
