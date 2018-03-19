#include <bits/stdc++.h>
#include <ctime>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
using namespace std;


#define BLOCK_SIZE 256
#define MAX_INODES 10000
#define MAX_DATA_BLOCKS 10000
#define PTRS_PER_BLOCK 64

struct super_block{
  int FS_SIZE;
  int num_free_data_blocks;
  bitset<MAX_DATA_BLOCKS> free_data_blocks;
  int max_num_inodes;
  bitset<MAX_INODES> free_inodes;

  void print_details(int details){
    cout << "FileSystem Size: " << this->FS_SIZE << endl;
    cout << "Number of Free Data Blocks: " << this->num_free_data_blocks << endl;
    if(details)
      cout << "Free Data Blocks: " << this->free_data_blocks << endl;
    cout << "Max Free Inodes: " << this->max_num_inodes << endl;
    if(details)
      cout << "Free Inodes: " << this->free_inodes << endl;
  }
};

struct file_data_ptr{
  int direct_pointer[10];
  int single_pointer;
  int double_pointer;
  int triple_pointer;

  file_data_ptr(){
    memset(direct_pointer, -1, sizeof(int)*10);
    single_pointer = -1;
    double_pointer = -1;
    triple_pointer = -1;
  }


  void print_details(){
    cout << "Direct Pointer: ";
    for(int i=0; i<10; i++){
      if(direct_pointer[i] != -1)
        cout << this->direct_pointer[i] << " ";
    }
    cout << endl;
    cout << "Single Pointer: " << this->single_pointer << endl;
    cout << "Double Pointer: " << this->double_pointer << endl;
    cout << "Triple Pointer: " << this->triple_pointer << endl;
  }
};

struct inode{
  int owner;
  int type;
  int epoch_last_accessed;
  int epoch_last_modified;
  int epoch_last_inode_modified;
  int file_size;
  int access_permissions;
  file_data_ptr fd_ptr;

  inode(){
    this->owner = -1;
    this->type = -1;
    this->epoch_last_accessed = -1;
    this->epoch_last_modified = -1;
    this->epoch_last_inode_modified = -1;
    this->file_size = 0;
    this->access_permissions = 777;
    file_data_ptr ptr;
    this->fd_ptr = ptr;
  }


  void make_inode(int isFolder){
    this->owner = 0;
    this->type = isFolder;
    this->epoch_last_accessed = time(0);
    this->epoch_last_modified = time(0);
    this->epoch_last_inode_modified = time(0);
    this->file_size = 0;
    this->access_permissions = 777;
    file_data_ptr ptr;
    this->fd_ptr = ptr;
  }


  void print_details(){
    cout << "Owner ID: " << this->owner << endl;
    cout << "Type: " << (this->type)? "Folder": "File";
    cout << endl;
    cout << "Last Accessed: " << this->epoch_last_accessed << endl;
    cout << "Last Modified: " << this->epoch_last_modified << endl;
    cout << "Last Inode Modified: " << this->epoch_last_inode_modified << endl;
    cout << "File Size: " << this->file_size << endl;
    cout << "Access Permissions: " << this->access_permissions << endl;
    cout << "File Pointer: " << endl;
    this->fd_ptr.print_details();
  }
};

struct inode_list{
  inode list[MAX_INODES];

  void print_details(){
    cout << "INODE DETAILS: " << endl;
    for(int i=0; i<MAX_INODES; i++){
      if(list[i].owner != -1){
        list[i].print_details();
        cout << endl;
      }
    }
  }
};


class MRFS{
private:
  uint8_t* memory;

  void init_super_block(int size){
    super_block sb;
    sb.FS_SIZE = size;
    sb.num_free_data_blocks = min(int((size - sizeof(sb) - sizeof(inode_list))/BLOCK_SIZE),
                                  MAX_DATA_BLOCKS);
    sb.free_data_blocks.reset();
    // Set unreachable blocks to 1
    for(int i=sb.num_free_data_blocks; i<MAX_DATA_BLOCKS; i++)
      sb.free_data_blocks.set(i, 1);

    sb.max_num_inodes = sb.num_free_data_blocks;
    sb.free_inodes.reset();

    // Set the inodes to true, which we can't access.
    for(int i=sb.num_free_data_blocks; i< MAX_INODES; i++)
      sb.free_inodes.set(i, 1);
    memcpy(this->memory, &sb, sizeof(sb));
  }


  int make_root_directory(){
    int i = this->get_super_blocks_count();
    i += this->get_inode_list_blocks_count();
    string s = ". 0\n.. 0\n";
    memcpy(this->memory + i*BLOCK_SIZE, s.c_str(), s.size());
    return s.size();
  }


  void init_inode_list(){
    inode_list ls;
    inode root;
    root.make_inode(1);
    root.fd_ptr.direct_pointer[0] = 0;

    ls.list[0] = root;

    // Initialise the root directory and store the file size
    ls.list[0].file_size = this->make_root_directory();

    // Write the inode list to the memory
    int i = this->get_super_blocks_count();
    memcpy(this->memory + i*BLOCK_SIZE, &ls, sizeof(ls));

    // Update the superblock
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.num_free_data_blocks--;
    sb.free_data_blocks.set(0, 1);
    sb.free_inodes.set(0, 1);
    memcpy(this->memory, &sb, sizeof(sb));

    this->current_working_directory = 0;
  }


  int init_file_table(){
    string folder_contents = this->get_file_contents(0);
    istringstream iss(folder_contents);
    string filename;
    int inode_num;
    while(!iss.eof()){
      iss >> filename >> inode_num;
      if(filename.compare(".filetable") == 0){
        return inode_num;
      }
    }
    int cwd = this->current_working_directory;
    this->current_working_directory = 0;
    inode_num = this->write_file("", ".filetable");
    folder_contents += ".filetable " + to_string(inode_num) + "\n";
    this->update_file(this->current_working_directory, folder_contents);
    this->current_working_directory = cwd;
    return inode_num;
  }

  string read_from(const char *src){
    ifstream inFile;
    inFile.open(src);
    string text;
    getline(inFile, text, (char) inFile.eof());
    inFile.close();
    return text;
  }


  string get_direct_pointer_contents(int direct_pointer[], int &bytes, int n=10){
    int i = this->get_super_blocks_count();
    i += this->get_inode_list_blocks_count();
    int j = 0;
    string file_contents = "";
    while(bytes != 0 && j<n){
      if(direct_pointer[j] >= 0){
        char ar[min(bytes, BLOCK_SIZE)];
        memset(ar, -1, min(bytes, BLOCK_SIZE)*sizeof(char));
        memcpy(ar, this->memory + (i + direct_pointer[j])* BLOCK_SIZE, min(bytes, BLOCK_SIZE));
        string temp(ar);
        temp = temp.substr(0, min(bytes, BLOCK_SIZE));
        file_contents += temp;
        bytes -= min(BLOCK_SIZE, bytes);
      }
      j++;
    }
    return file_contents;
  }


  string get_single_pointer_contents(int single_pointer, int &bytes){
    int i = this->get_super_blocks_count();
    i += this->get_inode_list_blocks_count();
    int j=i;
    i += single_pointer;
    int direct_pointer_block[PTRS_PER_BLOCK];
    memcpy(&direct_pointer_block, this->memory + i *BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
    string file_contents = "";
    file_contents += this->get_direct_pointer_contents(direct_pointer_block, bytes);
    return file_contents;
  }


  string get_double_pointer_contents(int double_pointer, int &bytes){
    int i = this->get_super_blocks_count();
    i += this->get_inode_list_blocks_count();
    int j=i;
    i += double_pointer;
    int single_pointer_block[PTRS_PER_BLOCK];
    memcpy(&single_pointer_block, this->memory + i *BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
    string file_contents = "";
    for(i=0; i<PTRS_PER_BLOCK; i++){
      if(single_pointer_block[i] != -1){
        file_contents += this->get_single_pointer_contents(single_pointer_block[i], bytes);
      }
    }
    return file_contents;
  }


  string get_triple_pointer_contents(int triple_pointer, int &bytes){
    int i = this->get_super_blocks_count();
    i += this->get_inode_list_blocks_count();
    int j=i;
    i += triple_pointer;
    int double_pointer_block[PTRS_PER_BLOCK];
    memcpy(&double_pointer_block, this->memory + i *BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
    string file_contents = "";
    for(i=0; i<PTRS_PER_BLOCK; i++){
      if(double_pointer_block[i] != -1){
        file_contents += this->get_double_pointer_contents(double_pointer_block[i], bytes);
      }
    }
    return file_contents;
  }


  void write_direct_pointers(string file_contents, int &bytes, int direct_pointer[], int n=10){
    super_block sb;
    memcpy(&sb, this->memory, sizeof(super_block));
    int j=0;
    int i=0;
    int x = this->get_super_blocks_count() + this->get_inode_list_blocks_count();
    while(bytes > 0 && j<n){
      for(; i<sb.free_data_blocks.size(); i++){
        if(sb.free_data_blocks.test(i) == 0){
          sb.free_data_blocks.set(i, 1);
          direct_pointer[j] = i;
          break;
        }
      }
      string temp = file_contents.substr(j*BLOCK_SIZE, BLOCK_SIZE);
      memcpy(this->memory + (x+i)*BLOCK_SIZE, temp.c_str(), temp.size());
      bytes -= temp.size();
      j++;
      sb.num_free_data_blocks--;
    }
    // Update Super Block
    memcpy(this->memory, &sb, sizeof(sb));
  }


  void write_single_pointer(string file_contents, int &bytes, int &single_pointer){
    int direct_pointer[PTRS_PER_BLOCK];
    memset(direct_pointer, -1, sizeof(int)*PTRS_PER_BLOCK);
    super_block sb;
    int i=0;
    int x = this->get_super_blocks_count();
    x += this->get_inode_list_blocks_count();
    memcpy(&sb, this->memory, sizeof(super_block));
    for(; i<sb.free_data_blocks.size(); i++){
      if(sb.free_data_blocks.test(i) == 0){
        sb.free_data_blocks.set(i, 1);
        single_pointer = i;
        sb.num_free_data_blocks--;
        break;
      }
    }
    // Update Super Block
    memcpy(this->memory, &sb, sizeof(sb));
    this->write_direct_pointers(file_contents, bytes, direct_pointer, PTRS_PER_BLOCK);
    memcpy(this->memory+(x+single_pointer)*BLOCK_SIZE, direct_pointer, sizeof(int)*PTRS_PER_BLOCK);
  }


  void write_double_pointer(string file_contents, int &bytes, int &double_pointer){
    int single_pointer[PTRS_PER_BLOCK];
    memset(single_pointer, -1, sizeof(int)*PTRS_PER_BLOCK);
    super_block sb;
    int i=0;
    int x = this->get_super_blocks_count();
    x += this->get_inode_list_blocks_count();
    memcpy(&sb, this->memory, sizeof(super_block));
    for(; i<sb.free_data_blocks.size(); i++){
      if(sb.free_data_blocks.test(i) == 0){
        sb.free_data_blocks.set(i, 1);
        double_pointer = i;
        sb.num_free_data_blocks--;
        break;
      }
    }
    // Update Super Block
    memcpy(this->memory, &sb, sizeof(sb));
    int j=0;
    while(bytes != file_contents.size() && j<PTRS_PER_BLOCK){
      string temp = file_contents.substr(j*PTRS_PER_BLOCK*BLOCK_SIZE, PTRS_PER_BLOCK*BLOCK_SIZE);
      this->write_single_pointer(temp, bytes, single_pointer[j]);
      j++;
    }
    memcpy(this->memory+(x+double_pointer)*BLOCK_SIZE, single_pointer, sizeof(int)*PTRS_PER_BLOCK);
  }


  void write_triple_pointer(string file_contents, int &bytes, int &triple_pointer){
    int double_pointer[PTRS_PER_BLOCK];
    memset(double_pointer, -1, sizeof(int)*PTRS_PER_BLOCK);
    super_block sb;
    int i=0;
    int x = this->get_super_blocks_count();
    x += this->get_inode_list_blocks_count();
    memcpy(&sb, this->memory, sizeof(super_block));
    for(; i<sb.free_data_blocks.size(); i++){
      if(sb.free_data_blocks.test(i) == 0){
        sb.free_data_blocks.set(i, 1);
        triple_pointer = i;
        sb.num_free_data_blocks--;
        break;
      }
    }
    // Update Super Block
    memcpy(this->memory, &sb, sizeof(sb));
    int j=0;
    while(bytes != file_contents.size() && j<PTRS_PER_BLOCK){
      string temp = file_contents.substr(j*PTRS_PER_BLOCK*PTRS_PER_BLOCK*BLOCK_SIZE,
         PTRS_PER_BLOCK*PTRS_PER_BLOCK*BLOCK_SIZE);
      this->write_double_pointer(temp, bytes, double_pointer[j]);
      j++;
    }
    memcpy(this->memory+(x+triple_pointer)*BLOCK_SIZE, double_pointer, sizeof(int)*PTRS_PER_BLOCK);
  }


  string get_file_contents(int inode_num){
    int i = this->get_super_blocks_count();
    inode_list ls;
    inode file_inode;
    memcpy(&ls, this->memory + (i)*BLOCK_SIZE, sizeof(inode_list));
    file_inode = ls.list[inode_num];
    string file_contents = "";

    int bytes = file_inode.file_size;

    file_contents += this->get_direct_pointer_contents(file_inode.fd_ptr.direct_pointer, bytes, 10);
    if(bytes > 0)
      file_contents += this->get_single_pointer_contents(file_inode.fd_ptr.single_pointer, bytes);
    if(bytes > 0)
      file_contents += this->get_double_pointer_contents(file_inode.fd_ptr.double_pointer, bytes);
    if(bytes > 0)
      file_contents += this->get_triple_pointer_contents(file_inode.fd_ptr.triple_pointer, bytes);

    return file_contents;
  }


  int write_file(string file_contents, string filename){
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    int i = this->get_super_blocks_count();

    // Create an inode
    inode_list ls;
    memcpy(&ls, this->memory+ i*BLOCK_SIZE, sizeof(ls));
    for(i=0; i<sb.max_num_inodes; i++){
      if(sb.free_inodes.test(i) == 0){
          sb.free_inodes.set(i, 1);
          break;
      }
    }
    // Update Inode List and Super Block
    memcpy(this->memory, &sb, sizeof(sb));

    inode new_inode;
    new_inode.make_inode(0);
    new_inode.file_size = file_contents.size();
    int bytes = file_contents.size();
    int j=0;
    int x = i;
    i=0;

    // Try with direct pointers first, then by single, double and triple pointers
    this->write_direct_pointers(file_contents, bytes, new_inode.fd_ptr.direct_pointer);
    if(bytes > 0){
      string temp = file_contents.substr(10*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, new_inode.fd_ptr.single_pointer);
    }
    if(bytes > 0){
      string temp = file_contents.substr((10+PTRS_PER_BLOCK)*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, new_inode.fd_ptr.double_pointer);
    }
    if(bytes > 0){
      string temp = file_contents.substr((10+PTRS_PER_BLOCK+PTRS_PER_BLOCK*PTRS_PER_BLOCK)*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, new_inode.fd_ptr.triple_pointer);
    }

    i = this->get_super_blocks_count();
    ls.list[x] = new_inode;
    memcpy(this->memory + i*BLOCK_SIZE, &ls, sizeof(inode_list));

    return x;
  }


  void write_inode_data(int inode_num, string file_contents){
    inode_list ls;
    int x = this->get_super_blocks_count();
    memcpy(&ls, this->memory+x*BLOCK_SIZE, sizeof(ls));
    inode file_inode = ls.list[inode_num];

    file_inode.file_size = file_contents.size();
    int bytes = file_contents.size();

    // Try with direct pointers first, then by single, double and triple pointers
    this->write_direct_pointers(file_contents, bytes, file_inode.fd_ptr.direct_pointer);
    if(bytes > 0){
      string temp = file_contents.substr(10*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, file_inode.fd_ptr.single_pointer);
    }
    if(bytes > 0){
      string temp = file_contents.substr((10+PTRS_PER_BLOCK)*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, file_inode.fd_ptr.double_pointer);
    }
    if(bytes > 0){
      string temp = file_contents.substr((10+PTRS_PER_BLOCK+PTRS_PER_BLOCK*PTRS_PER_BLOCK)*BLOCK_SIZE, file_contents.size());
      this->write_single_pointer(temp, bytes, file_inode.fd_ptr.triple_pointer);
    }

    ls.list[inode_num] = file_inode;
    memcpy(this->memory + x*BLOCK_SIZE, &ls, sizeof(inode_list));
  }

  bool check_file_unique(string folder_contents, string newfilename){
    istringstream iss(folder_contents);
    string filename, temp;
    while(!iss.eof()){
      iss >> filename >> temp;
      if(filename.compare(newfilename) == 0)
        return false;
    }
    return true;
  }

  void remove_direct_pointer(int ar[], int n=10){
    //Get super block
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));

    for(int i=0; i<n; i++){
      if(ar[i] < 0)
        continue;
      sb.free_data_blocks.set(ar[i], 0);
      ar[i] = -1;
    }

    // Update superblock
    memcpy(this->memory, &sb, sizeof(sb));
  }

  void remove_single_pointer(int &single_pointer){
    if(single_pointer > 0){
      int x = this->get_super_blocks_count();
      super_block sb;
      memcpy(&sb, this->memory, sizeof(sb));
      sb.free_data_blocks.set(single_pointer, 0);
      memcpy(this->memory, &sb, sizeof(sb));

      x += this->get_inode_list_blocks_count();
      int direct_pointer[PTRS_PER_BLOCK];
      memcpy(direct_pointer, this->memory+(x+single_pointer)*BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
      this->remove_direct_pointer(direct_pointer, PTRS_PER_BLOCK);
      single_pointer = -1;
    }
  }

  void remove_double_pointer(int &double_pointer){
    if(double_pointer < 0)
      return;
    int x = this->get_super_blocks_count();
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.free_data_blocks.set(double_pointer, 0);
    memcpy(this->memory, &sb, sizeof(sb));

    x += this->get_inode_list_blocks_count();
    int single_pointer[PTRS_PER_BLOCK];
    memcpy(single_pointer, this->memory+(x+double_pointer)*BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
    for(int i=0; i<PTRS_PER_BLOCK; i++)
      this->remove_single_pointer(single_pointer[i]);
    double_pointer = -1;
  }

  void remove_triple_pointer(int &triple_pointer){
    if(triple_pointer < 0)
      return;
    int x = this->get_super_blocks_count();
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.free_data_blocks.set(triple_pointer, 0);
    memcpy(this->memory, &sb, sizeof(sb));

    x += this->get_inode_list_blocks_count();
    int double_pointer[PTRS_PER_BLOCK];
    memcpy(double_pointer, this->memory+(x+triple_pointer)*BLOCK_SIZE, sizeof(int)*PTRS_PER_BLOCK);
    for(int i=0; i<PTRS_PER_BLOCK; i++)
      this->remove_double_pointer(double_pointer[i]);
    triple_pointer = -1;
  }

  int remove_file_data(int inode_num){
    // Get inode list
    inode_list ls;
    int x = this->get_super_blocks_count();
    memcpy(&ls, this->memory + x*BLOCK_SIZE, sizeof(ls));

    // Remove data
    inode file_inode = ls.list[inode_num];
    this->remove_direct_pointer(file_inode.fd_ptr.direct_pointer);
    this->remove_single_pointer(file_inode.fd_ptr.single_pointer);
    this->remove_double_pointer(file_inode.fd_ptr.double_pointer);
    this->remove_triple_pointer(file_inode.fd_ptr.triple_pointer);
    ls.list[inode_num] = file_inode;
    memcpy(this->memory+x*BLOCK_SIZE, &ls, sizeof(ls));
  }

  int remove_file(int inode_num){
    this->remove_file_data(inode_num);
    inode_list ls;
    int x = this->get_super_blocks_count();
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.free_inodes.set(inode_num, 0);
    memcpy(this->memory, &sb, sizeof(sb));

    memcpy(&ls, this->memory + x*BLOCK_SIZE, sizeof(ls));
    inode temp;
    ls.list[inode_num] = temp;
    memcpy(this->memory+x*BLOCK_SIZE, &ls, sizeof(ls));
    return 0;
  }


  void update_file(int inode_num, string new_file_contents){
    this->remove_file_data(inode_num);
    this->write_inode_data(inode_num, new_file_contents);
  }


  int filename2inodenum(string src){
    string folder_contents = this->get_file_contents(this->current_working_directory);
    int inode_num = -1;
    int temp_num = -1;
    string temp;
    istringstream iss(folder_contents);
    while(!iss.eof()){
      iss >> temp >> temp_num;
      if(temp.compare(src) == 0){
          return temp_num;
      }
    }
    return inode_num;
  }


public:
  int current_working_directory;

  MRFS(){
    this->memory = NULL;
  }


  int create_MRFS(int size){
    int true_size = min((long unsigned int)size,
      sizeof(super_block) +
      sizeof(inode_list) +
      BLOCK_SIZE * MAX_DATA_BLOCKS);
    this->memory = (uint8_t*)malloc(true_size);
    if(this->memory == NULL)
      return -1;
    else {
      this->init_super_block(true_size);
      this->init_inode_list();
    }
    return true_size;
  }


  int copy_pc2myfs(string src, string dest){
    string file_contents = "";
    if(src.size() != 0)
      file_contents = this->read_from(src.c_str());

    // Get the inode first
    inode_list ls;
    int i = this->get_super_blocks_count();
    memcpy(&ls, this->memory + i*BLOCK_SIZE, sizeof(ls));
    inode cwd = ls.list[this->current_working_directory];
    if(cwd.type != 1)
      return -1;

    string folder_contents = this->get_file_contents(this->current_working_directory);
    if(this->check_file_unique(folder_contents, dest)){
      int inode_num = this->write_file(file_contents, dest);
      folder_contents += dest.c_str() + string(" ") + to_string(inode_num) + "\n";
      this->update_file(this->current_working_directory, folder_contents);
    } else {
      cout << "File already exists" << endl;
      return -1;
    }
    return 0;
  }


  int copy_myfs2pc(string src, string dest){
    int inode_num = this->filename2inodenum(src);
    string file_contents = this->get_file_contents(inode_num);
    ofstream fs;
    fs.open(dest.c_str());
    fs << file_contents;
    fs.close();
    return 0;
  }

  int rm_myfs(string filename){
    int inode_num = this->filename2inodenum(filename);
    int x = this->remove_file(inode_num);
    if(x < 0){
      return -1;
    }
    string folder_contents = this->get_file_contents(this->current_working_directory);
    string rem_string = filename + " " + to_string(inode_num) + "\n";
    int pos = folder_contents.find(rem_string);
    if(pos!=string::npos){
      folder_contents.erase(pos, rem_string.size());
    }
    this->update_file(this->current_working_directory, folder_contents);
  }


  int showfile_myfs(string filename){
    int inode_num = this->filename2inodenum(filename);
    cout << this->get_file_contents(inode_num);
    return 0;
  }


  int ls_myfs(){
    string folder_contents = this->get_file_contents(this->current_working_directory);
    cout << folder_contents;
    return 0;
  }


  int mkdir_myfs(string dirname){
    string folder_contents = this->get_file_contents(this->current_working_directory);
    if(this->check_file_unique(folder_contents, dirname)){
      //create a new inode and save it.
      int i=-1;
      super_block sb;
      memcpy(&sb, this->memory, sizeof(sb));
      for(i=0; i<sb.max_num_inodes; i++){
        if(sb.free_inodes.test(i) == 0){
          sb.free_inodes.set(i, 1);
          break;
        }
      }
      memcpy(this->memory, &sb, sizeof(sb));
      if(i<0){
        cout << "Blocks full" << endl;
        return -1;
      }
      int x = this->get_super_blocks_count();
      inode_list ls;
      memcpy(&ls, this->memory+x*BLOCK_SIZE, sizeof(ls));

      inode new_folder_inode;
      new_folder_inode.make_inode(1);
      string folder_init = ". "+to_string(i)+"\n.. "+to_string(this->current_working_directory)+"\n";
      new_folder_inode.file_size = folder_init.size();
      ls.list[i] = new_folder_inode;
      memcpy(this->memory+x*BLOCK_SIZE, &ls, sizeof(ls));
      this->update_file(i, folder_init);

      string existing_folder_content = this->get_file_contents(this->current_working_directory);
      existing_folder_content += dirname + " " + to_string(i) + "\n";
      this->update_file(this->current_working_directory, existing_folder_content);
    } else {
      cout << "Folder already exists" << endl;
      return -1;
    }
    return 0;
  }

  int chdir_myfs(string dirname){
      int inode_num = this->filename2inodenum(dirname);
      if(inode_num < 0){
        cout << "Non-existent" << endl;
        return -1;
      }
      inode_list ls;
      memcpy(&ls, this->memory+this->get_super_blocks_count()*BLOCK_SIZE, sizeof(ls));
      if(ls.list[inode_num].type != 1){
        cout << "Not a file." << endl;
        return -1;
      }
      this->current_working_directory = inode_num;
      return 0;
  }


  int rmdir_myfs(string dirname){
    int temp_int;
    string folder_contents = this->get_file_contents(this->current_working_directory);
    string temp;
    int inode_num = -1;
    istringstream iss(folder_contents);
    folder_contents = "";
    while(!iss.eof()){
      iss >> temp >> temp_int;
      if(temp.compare(dirname) == 0){
        inode_num = temp_int;
        continue;
      } else {
        folder_contents += temp +" "+to_string(temp_int)+"\n";
      }
    }
    if(inode_num < 0){
      cout << "Folder non-existent" << endl;
      return -1;
    }
    inode_list ls;
    memcpy(&ls,this->memory+this->get_super_blocks_count()*BLOCK_SIZE,sizeof(ls));
    if(ls.list[inode_num].type != 1){
      cout << "Not a folder" << endl;
      return -1;
    }
    string del_folder_contents = this->get_file_contents(inode_num);
    if(del_folder_contents.size() > 9){
      cout << "Folder is non-empty. Remove the files inside first" << endl;
      return -1;
    }
    this->update_file(this->current_working_directory, folder_contents);
    return this->remove_file(inode_num);
  }


  int dump_myfs(string dest){
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    ofstream fs;
    fs.open(dest.c_str());
    for(int i=0; i<sb.FS_SIZE; i++)
      fs << this->memory[i];
    fs.close();
    return 0;
  }


  int restore_myfs(string src){
    cout << "Restoring. If the two file systems didn't have same size, problems might occur!" << endl;
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    FILE *fp;
    this->current_working_directory = 0;
    fp = fopen(src.c_str(),"rb");
    if (fp == NULL){
      cout << "Backup not found." << endl;
      return -1;
    }
    if(fread(this->memory, sizeof(uint8_t), sb.FS_SIZE, fp) != sb.FS_SIZE){
      cout << "Some eror may be expected" << endl;
      return -1;
    }
    return 0;
  }


  int status_myfs(int details = 0, int inode_content = 0){
    this->get_super_blocks_content(details);
    if(inode_content)
      this->get_inode_list_content();
    return 0;
  }


  int chmod_myfs(string filename, int mod){
    int inode_num = this->filename2inodenum(filename);
    if(inode_num < -1){
      cout << "Non existent file/folder" << endl;
      return -1;
    }
    inode_list ls;
    int x = this->get_super_blocks_count();
    memcpy(&ls, this->memory+x*BLOCK_SIZE, sizeof(ls));
    if(mod > 777 || mod < 0 || mod%10 > 7 || (int(mod/10))%10 > 7 || (int(mod/100))%10 > 7){
      cout << "Wrong permissions!" << endl;
      return -1;
    }
    ls.list[inode_num].access_permissions = mod;
    memcpy(this->memory+x*BLOCK_SIZE, &ls, sizeof(ls));
    return 0;
  }


  int open_myfs(string filename, char mode){
    int inode_num = this->filename2inodenum(filename);
    if(inode_num < 0){
      inode_num = this->copy_pc2myfs("", filename);
      inode_num = this->filename2inodenum(filename);
      if(inode_num < 0)
        return -1;
    }
    int filetable_inode = this->init_file_table();
    string filetable_contents = this->get_file_contents(filetable_inode);
    int fd=count(filetable_contents.begin(),filetable_contents.end(),'\n');
    if(mode == 'r')
      filetable_contents += to_string(fd) + " " + to_string(inode_num) + " 0 -1\n";
    else
      filetable_contents += to_string(fd) + " " + to_string(inode_num) + " -1 0\n";
    this->update_file(filetable_inode, filetable_contents);
    return fd;
  }


  int close_myfs(int fd){
    int filetable_inode = this->init_file_table();
    string filetable_contents = this->get_file_contents(filetable_inode);
    istringstream iss(filetable_contents);
    string line;
    filetable_contents = "";
    int count = 0;
    while(getline(iss, line)){
      if(count != fd){
        filetable_contents += line+"\n";
      }
      count++;
    }
    if(filetable_contents.size() == 0){
      this->remove_file(filetable_inode);
      return 0;
    }
    this->update_file(filetable_inode, filetable_contents);
    return 0;
  }


  int read_myfs(int fd, int nbytes, char *buf){
    int filetable_inode = this->init_file_table();
    string filetable_contents = this->get_file_contents(filetable_inode);
    istringstream iss(filetable_contents);
    int fd_num, inode_num, read_byte, write_byte;
    fd_num = -1;
    while(!iss.eof()){
      iss >> fd_num >> inode_num >> read_byte >> write_byte;
      if(fd_num == fd)
        break;
    }
    if(fd_num < 0){
      return -1;
    }
    string existing_string = to_string(fd_num) + " " + to_string(inode_num) + " ";
    existing_string += to_string(read_byte) + " " + to_string(write_byte) + "\n";
    if(read_byte < 0){
      cout << "Cannot read in write mode." << endl;
      return -1;
    }
    string res = this->get_file_contents(inode_num);
    int ans;
    inode_list ls;
    memcpy(&ls, this->memory+this->get_super_blocks_count()*BLOCK_SIZE, sizeof(ls));
    if(ls.list[inode_num].file_size >= nbytes + read_byte){
      res = res.substr(read_byte, nbytes);
      memcpy(buf, res.c_str(), nbytes);
      read_byte += nbytes;
      ans = read_byte;
    } else {
      res = res.substr(read_byte, ls.list[inode_num].file_size - read_byte);
      memcpy(buf, res.c_str(), ls.list[inode_num].file_size - read_byte);
      read_byte = ls.list[inode_num].file_size;
      ans = read_byte;
    }
    string new_string = to_string(fd) + " " + to_string(inode_num) + " ";
    new_string += to_string(read_byte) + " " + to_string(write_byte) + "\n";
    filetable_contents.replace(filetable_contents.find(existing_string),existing_string.length(),new_string);

    // Update filetable
    this->update_file(filetable_inode, filetable_contents);
    return ans;
  }


  int write_myfs(int fd, int nbytes, char *buf){
    int filetable_inode = this->init_file_table();
    string filetable_contents = this->get_file_contents(filetable_inode);
    istringstream iss(filetable_contents);
    int fd_num, inode_num, read_byte, write_byte;
    fd_num = -1;
    while(!iss.eof()){
      iss >> fd_num >> inode_num >> read_byte >> write_byte;
      if(fd_num == fd)
        break;
    }
    if(fd_num < 0){
      return -1;
    }
    string existing_string = to_string(fd_num) + " " + to_string(inode_num) + " ";
    existing_string += to_string(read_byte) + " " + to_string(write_byte) + "\n";
    if(write_byte < 0){
      cout << "Cannot write in read mode." << endl;
      return -1;
    }
    string res = this->get_file_contents(inode_num);
    inode_list ls;
    memcpy(&ls, this->memory+this->get_super_blocks_count()*BLOCK_SIZE, sizeof(ls));
    inode file_inode = ls.list[inode_num];

    std::ostringstream buffer;
    for(int j=0; j<nbytes; j++)
      buffer << buf[j];
    res.replace(write_byte, nbytes, buffer.str());

    file_inode.file_size = res.size();
    ls.list[inode_num] = file_inode;
    write_byte += nbytes;
    this->update_file(inode_num, res);
    string new_string = to_string(fd) + " " + to_string(inode_num) + " ";
    new_string += to_string(read_byte) + " " + to_string(write_byte) + "\n";
    filetable_contents.replace(filetable_contents.find(existing_string),existing_string.length(),new_string);

    // Update filetable
    this->update_file(filetable_inode, filetable_contents);
    return nbytes;
  }


  int eof_myfs(int fd){
    int filetable_inode = this->init_file_table();
    string filetable_contents = this->get_file_contents(filetable_inode);
    istringstream iss(filetable_contents);
    int fd_num, inode_num, read_byte, write_byte;
    fd_num = -1;
    while(!iss.eof()){
      iss >> fd_num >> inode_num >> read_byte >> write_byte;
      if(fd_num == fd)
        break;
    }
    if(fd_num < 0){
      return -1;
    }

    inode_list ls;
    memcpy(&ls, this->memory+this->get_super_blocks_count()*BLOCK_SIZE, sizeof(ls));
    inode file_inode = ls.list[inode_num];
    if(file_inode.file_size == read_byte || file_inode.file_size == write_byte){
      return 1;
    } else{
      return 0;
    }
    return -1;
  }


  int get_super_blocks_count(){
      int i = sizeof(super_block) % BLOCK_SIZE ?
              1+ sizeof(super_block)/BLOCK_SIZE:
              sizeof(super_block)/BLOCK_SIZE;
      return i;
  }


  int get_inode_list_blocks_count(){
      int i = sizeof(inode_list) % BLOCK_SIZE ?
              1+ sizeof(inode_list)/BLOCK_SIZE:
              sizeof(inode_list)/BLOCK_SIZE;
      return i;
  }


  void get_super_blocks_content(int details){
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.print_details(details);
  }


  void get_inode_list_content(){
    inode_list ls;
    int i = this->get_super_blocks_count();
    memcpy(&ls, this->memory+ i*BLOCK_SIZE, sizeof(ls));
    ls.print_details();
  }

  void debugger(){
    inode_list ls;
    memcpy(&ls, this->memory+this->get_super_blocks_count()*BLOCK_SIZE, sizeof(ls));
    ls.list[1].print_details();
  }
};
