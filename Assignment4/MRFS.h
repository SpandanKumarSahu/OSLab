#include <bits/stdc++.h>
#include <ctime>
using namespace std;

#define BLOCK_SIZE 256
#define MAX_INODES 10000
#define MAX_DATA_BLOCKS 10000

struct super_block{
  int FS_SIZE;
  int num_free_data_blocks;
  bitset<MAX_DATA_BLOCKS> free_data_blocks;
  int max_num_inodes;
  bitset<MAX_INODES> free_inodes;

  void print_details(){
    cout << "FileSystem Size: " << this->FS_SIZE << endl;
    cout << "Number of Free Data Blocks: " << this->num_free_data_blocks << endl;
    cout << "Free Data Blocks: " << this->free_data_blocks << endl;
    cout << "Max Free Inodes: " << this->max_num_inodes << endl;
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
    string s = ". 0\n.. 0";
    memcpy(this->memory + i*BLOCK_SIZE, &s, sizeof(s));
    return sizeof(s);
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


  string read_from(const char *src){
    ifstream inFile;
    inFile.open(src);
    string text;
    getline(inFile, text, (char) inFile.eof());
    inFile.close();
    return text;
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


  string get_file_contents(inode dir){
    int bytes = 0;
    while(bytes != dir.file_size){

    }
  }


  int copy_pc2myfs(string src, string dest){
    string file_contents = this->read_from(src.c_str());

    // Get the inode first
    inode_list ls;
    int i = this->get_super_blocks_count();
    memcpy(&ls, this->memory + i*BLOCK_SIZE, sizeof(ls));
    inode cwd = ls[this->current_working_directory];
    if(cwd.type != 1)
      return -1;

    string folder_contents = this->get_file_contents(cwd);

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

  void get_super_blocks_content(){
    super_block sb;
    memcpy(&sb, this->memory, sizeof(sb));
    sb.print_details();
  }


  void get_inode_list_content(){
    inode_list ls;
    int i = this->get_super_blocks_count();
    memcpy(&ls, this->memory+ i*BLOCK_SIZE, sizeof(ls));
    ls.print_details();
  }
};
