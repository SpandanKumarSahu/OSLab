#include <bits/stdc++.h>
#include <iostream>
#include <set>
#include <iterator>
#include <cstdlib>
using namespace std;

#define MAX_DIFF 10
const float READ_WRITE_PROB = 0.8;

int total_pages, num_page_ref, set_size;
std::vector<bool> read_write;
std::vector<int> page_ref;

float myrand(){
  float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  return r;
}

std::vector<int> initialise_working_set(){
  set <int, greater <int> > ref;
  int page_num;
  while(ref.size() < set_size){
    page_num = rand() % total_pages;
    ref.insert(page_num);
  }
  set <int, greater <int> > :: iterator itr;
  std::vector<int> working_set;
  for (itr = ref.begin(); itr != ref.end(); ++itr)
  {
      working_set.push_back(*itr);
  }
  sort(working_set.begin(), working_set.end());
  return working_set;
}

int select_page_from_set(int page_index){
  int start_page = max(0, page_index - MAX_DIFF);
  int end_page = min(set_size, page_index + MAX_DIFF);
  return min(set_size, start_page + rand() % (end_page - start_page));
}

int main(){
  /* Get parameters */
  float change_prob;
  cout << "Enter total number of pages: " ;
  cin >> total_pages;
  cout << "Enter number of page references: ";
  cin >> num_page_ref;
  cout << "Enter working set size: ";
  cin >> set_size;
  cout << "Enter probability factor: ";
  cin >> change_prob;

  /* Initialise a working set */
  if(set_size > total_pages){
    cout << "Working set size couldn't be more than total number of pages" << endl;
    return -1;
  }
  srand(time(NULL));

  int curr_set_size = 0;
  std::vector<int> working_set = initialise_working_set();

  /* Generate a trace */
  int page_index = 0;
  int page_num = working_set[page_index];
  for(int i = 0; i < num_page_ref; i++){
    if(i%(num_page_ref/10) == 0)
      cout << i << " iterations complete." << endl;
    float temp = myrand();
    if(temp > change_prob){
      // Remove the page
      page_num = working_set[page_index];
      working_set.erase(working_set.begin()+page_index);

      // Add a new page
      int tries = 1;
      while(tries){
        // Get new page number
        page_num = min(total_pages, max(page_num - MAX_DIFF, 0) + rand() % (2*MAX_DIFF));

        // See if it already exists in the working set
        if (!std::binary_search(working_set.begin(), working_set.end(), page_num)){
          working_set.push_back(page_num);
          sort(working_set.begin(), working_set.end());
          break;
        }
        tries--;
      }
      if(!tries){
        // If all the neighbourhood pages are already present
        while(1){
          page_num = rand() % total_pages;
          if (!std::binary_search(working_set.begin(), working_set.end(), page_num)){
            working_set.push_back(page_num);
            sort(working_set.begin(), working_set.end());
            break;
          }
        }
      }
      page_index = find(working_set.begin(), working_set.end(), page_num) - working_set.begin();
    }
    else {
      page_index = select_page_from_set(page_index);
    }

    page_ref.push_back(working_set[page_index]);
    // write/read probability
    temp = myrand();
    if(temp < READ_WRITE_PROB)
      read_write.push_back(true);
    else
      read_write.push_back(false);
  }

  ofstream outfile;
  outfile.open("trace.txt");
  for(int i=0; i<num_page_ref; i++){
    // cout << read_write[i] << " " << page_ref[i] << endl;
    outfile << read_write[i] << " " << page_ref[i] << endl;
  }
  outfile.close();
  return 0;
}
