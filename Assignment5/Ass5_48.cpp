#include <bits/stdc++.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#define MAX_TABLE_SIZE 64
#define MEM_CPU_TIME 1
#define SWAPDEV_CPU_TIME 3000
#define MAP_CPU_TIME 250
#define n 64
#define num_algos 5

using namespace std;

int TABLE_SIZE;
int memory_map[MAX_TABLE_SIZE];
bool trace;

int page_operation;
int page_faults;
int CPU_time;

int count_algos;
string algo_name(){
  switch(count_algos){
    case 0:
    return "FIFO";
    break;
    case 1:
    return "RANDOM";
    break;
    case 2:
    return "LRU";
    break;
    case 3:
    return "NRU";
    break;
    case 4:
    return "SECOND CHANCE";
    break;
    default:
    return "WHAT?";
  }
}


queue<int> FIFO;
int timestamps[MAX_TABLE_SIZE];

/*
  Assumptions:
  1. Page numbers can be from 0 to 63.
*/

/* TABLE DESIGN
  MSB                                                                                           LSB
   ------------------------------------------------------------------------------------------------
  |                  Page Num (Max 29bits)                    | ValidBit | DirtyBit | ReferenceBit |
   ------------------------------------------------------------------------------------------------
*/

void parse_index_entry(int entry, int &pagenum, bool &isValid, bool &isDirty, bool &isReferenced){
  isValid = entry & 1<<2;
  isDirty = entry & 1<<1;
  isReferenced = entry & 1<<0;
  pagenum = (entry >> 3);
}

void get_mapping(int page, int &index_num, bool &isValid, bool &isDirty, bool &isReferenced){
  int pagenum;
  index_num = -1;
  for(int i=0; i<TABLE_SIZE; i++){
    parse_index_entry(memory_map[i], pagenum, isValid, isDirty, isReferenced);
    if(pagenum == page && isValid == true){
        index_num = i;
        CPU_time += MEM_CPU_TIME;
        return;
    }
  }
  CPU_time += MEM_CPU_TIME;
}

int get_replacement_index(){
  int pagenum, index_num;
  bool isValid, isDirty, isReferenced;
  for(int i=0; i<TABLE_SIZE; i++){
    parse_index_entry(memory_map[i], pagenum, isValid, isDirty, isReferenced);
    if(isValid == false)
      return i;
  }
  if(count_algos == 0){
    /* FIFO */
    if(FIFO.empty()){
      cout << "Grave error!" << endl;
      exit(0);
    } else{
      index_num = FIFO.front();
      FIFO.pop();
      return index_num;
    }
  } else if(count_algos == 1){
    /* Random */
    index_num = rand()%TABLE_SIZE;
    return index_num;
  } else if(count_algos == 2){
    /* LRU */
    index_num = 0;
    for(int i=0; i<TABLE_SIZE; i++){
      if(timestamps[i] < timestamps[index_num])
        index_num = i;
    }
    return index_num;
  } else if(count_algos == 3){
    /* NRU
      For detailed algorithm description, visit:
      https://en.wikipedia.org/wiki/Page_replacement_algorithm#Variants_on_LRU
    */
    std::vector<int> class_one, class_two, class_three, class_zero;
    for(int i=0; i<TABLE_SIZE; i++){
      parse_index_entry(memory_map[i], pagenum, isValid, isDirty, isReferenced);
      if(isReferenced && isDirty)
        class_three.push_back(i);
      else if(isReferenced && !isDirty)
        class_two.push_back(i);
      else if(!isReferenced && isDirty)
        class_one.push_back(i);
      else{
        class_zero.push_back(i);
        break;
      }
    }
    if(class_zero.size() > 0)
      return class_zero[0];
    else if(class_one.size() > 0)
      return class_one[0];
    else if(class_two.size() > 0)
      return class_two[0];
    else if(class_three.size() > 0)
      return class_three[0];
    else
      return 0;
  } else if(count_algos == 4){
    /* Second chance */
    if(FIFO.empty()){
      cout << "Grave error!" << endl;
      exit(0);
    } else{
      isReferenced = true;
      while(isReferenced == true){
        index_num = FIFO.front();
        parse_index_entry(memory_map[index_num], pagenum, isValid, isDirty, isReferenced);
        if(isReferenced == false)
          return index_num;
        else{
          memory_map[index_num] = pagenum << 3 | (1 << 2) | (isDirty << 1);
          FIFO.pop();
          FIFO.push(index_num);
        }
      }
    }
  }
}

int get_new_address(int pagenum){
  int index = get_replacement_index();
  bool isValid, isDirty, isReferenced;
  int pagenum_c;
  parse_index_entry(memory_map[index], pagenum_c, isValid, isDirty, isReferenced);
  if(isValid){
    if(isDirty){
      if(trace){
        cout << "UNMAP: " << pagenum_c << " " << index << endl;
        cout << "OUT: " << pagenum_c << " " << index << endl;
      }
      CPU_time += SWAPDEV_CPU_TIME + MAP_CPU_TIME;
    }
    else {
      if(trace)
        cout << "UNMAP: " << pagenum_c << " " << index << endl;
      CPU_time += MAP_CPU_TIME;
    }
  }
  if(trace){
    cout << "IN: " << pagenum << " " << index << endl;
    cout << "MAP: " << pagenum << " "  << index << endl;
  }
  CPU_time += SWAPDEV_CPU_TIME + MAP_CPU_TIME;
  page_operation += 1;
  memory_map[index] = (pagenum << 3) | 1<<0;
  FIFO.push(index);
  return index;
}

void get_file_contents(std::vector<bool> &type, std::vector<int> &pagenum, std::vector<int> &lines, string filename = "input.txt"){
  ifstream infile(filename.c_str());
  string text = "";
  string line;
  int linenum = 0;
  while(std::getline(infile, line)){
    if(line[0] == '#'){
      linenum++;
      continue;
    }
    std::istringstream iss(line);
    int a, b;
    if (!(iss >> a >> b))
      break;
    type.push_back(a);
    pagenum.push_back(b);
    lines.push_back(linenum);
    linenum++;
  }
}

int main(){
  cout << "Enter number of pages in memory" << endl;
  cin >> TABLE_SIZE;
  if(n > 1<<29){
    cout << "Page Table architecture doesn't support this number of pages" << endl;
    exit(0);
  }
  if(TABLE_SIZE > MAX_TABLE_SIZE){
    cout << "Page Table architecture doesn't support this number of pages" << endl;
    exit(0);
  }

  cout << "Enable tracing?(Y/n): ";
  char c;
  cin >> c;
  if(c=='y' || c=='Y'){
    trace = true;
  }

  // Get the instructions
  vector<int> pagenum;
  std::vector<bool> type;
  std::vector<int> lines;
  get_file_contents(type, pagenum, lines, "trace.txt");
  count_algos = 0;
  srand(time(NULL));
  int timer;

  // Initialise the memory mapping
  while(count_algos < num_algos){
    memset(memory_map, 0, TABLE_SIZE * sizeof(int));
    memset(timestamps, 0, TABLE_SIZE * sizeof(int));
    CPU_time = 0;
    page_operation = 0;
    page_faults = 0;
    timer = 0;

    for(int i=0; i<pagenum.size(); i++){
      // Get index number
      int index_num = -1;
      bool isValid, isDirty, isReferenced;
      if(trace)
        cout << "\n" << lines[i] << endl;
      if(pagenum[i] >= n){
        cout << "Page Fault Exception" << endl;
        page_faults += 1;
        continue;
      }
      get_mapping(pagenum[i], index_num, isValid, isDirty, isReferenced);

      if(index_num < 0){
        index_num = get_new_address(pagenum[i]);
      }

      if(type[i] == 0){
        /* Read type */
        memory_map[index_num] = pagenum[i] << 3 | (1 << 2) | (isDirty << 1) | (1<<0);
        CPU_time += MEM_CPU_TIME;
        timestamps[index_num] = ++timer;
      } else {
        /* Write type */
        memory_map[index_num] = pagenum[i] << 3 | (1 << 2) | (1<<1) | (1<<0);
        CPU_time += MEM_CPU_TIME;
        timestamps[index_num] = ++timer;
      }
      if(trace)
        cout << "\n";
    }
    cout << algo_name() << endl;
    cout << "Total out-of-bound pages: " << page_faults << endl;
    cout << "Total page faults: " << page_operation << endl;
    cout << "Total time taken: " << CPU_time << endl;
    cout << endl;
    count_algos++;
  }

  return 0;
}
