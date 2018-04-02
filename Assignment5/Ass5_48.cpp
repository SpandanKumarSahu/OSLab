#include <bits/stdc++.h>
#include <fstream>
#define MAX_TABLE_SIZE 64
#define MEM_CPU_TIME 1
#define SWAPDEV_CPU_TIME 3000
#define MAP_CPU_TIME 250
#define n 64

using namespace std;

int TABLE_SIZE;
int memory_map[MAX_TABLE_SIZE];
bool trace;

int page_operation;
int page_faults;
int CPU_time;


queue<int> FIFO;

/* TABLE DESIGN
  MSB                                                                                           LSB
   ------------------------------------------------------------------------------------------------
  |                  Page Num (Max 29bits)                    | ValidBit | DirtyBit | ReferenceBit |
   ------------------------------------------------------------------------------------------------
*/

void parse_index_entry(int entry, int &pagenum, bool &isValid, bool &isDirty, bool &isReferenced){
  isValid = entry & 1<<0;
  isDirty = entry & 1<<1;
  isReferenced = entry & 1<<2;
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
  if(FIFO.empty()){
    cout << "Grave error!" << endl;
    exit(0);
  } else{
    index_num = FIFO.front();
    FIFO.pop();
    return index_num;
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
  get_file_contents(type, pagenum, lines, "input.txt");

  // Initialise the memory mapping
  memset(memory_map, 0, TABLE_SIZE * sizeof(int));
  CPU_time = 0;
  page_operation = 0;
  page_faults = 0;

  for(int i=0; i<pagenum.size(); i++){
    // Get index number
    int index_num = -1;
    bool isValid, isDirty, isReferenced;
    if(trace)
      cout << "\n" << lines[i] << endl;
    if(pagenum[i] > n){
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
    } else {
      /* Write type */
      memory_map[index_num] = pagenum[i] << 3 | (1 << 2) | (1<<1) | (1<<0);
      CPU_time += MEM_CPU_TIME;
    }
    if(trace)
      cout << "\n";
  }
  cout << "Total out-of-bound pages: " << page_faults << endl;
  cout << "Total page faults: " << page_operation << endl;
  cout << "Total time taken: " << CPU_time << endl;
  return 0;
}