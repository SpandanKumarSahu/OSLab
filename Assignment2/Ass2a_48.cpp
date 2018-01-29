#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <signal.h>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

#define SHMSZ 30
#define DELIM ','
#define NUM_WORDS 5
#define WORD_LENGTH 6


int isPrime(int r)
{
  for(int i=2;i<r/2;i++)
    {
      if(r%i==0)
	return 0;
    }
  return 1;
}

long long generate_prime()
{
  int r;
  srand(time(NULL));
  while(true) {
    r = rand()%100;
    if(isPrime(r)) break;
  }
  return r;
}

bool isFilled(char* &s){
  int count = 0;
  while(*s != '\0' && count < SHMSZ){
    count++;
    s++;
  }
  if(count < SHMSZ)
    return false;
  else
    return true;
}

bool isEmpty(char* s){
  int count = 0;
  while(*s != '\0' && count < SHMSZ){
    s++;
    count++;
  }
  if(count == 0)
    return true;
  else
    return false;
}

void producer(int producerNum)
{
  int shmid;
  key_t key;
  char *shm, *s;
  // key specified by the producer
  key = 5678;

  // get shared segment
  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  // attach shared segment to the data region
  if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
    exit(1);
  }

  // Write to the shared memory;
  while(true){
    s = shm;
    srand(time(NULL));
    int r = rand()%5;
    sleep(r);
    if(isFilled(s))
      continue;
    int prime = generate_prime();
    cout << "Producer "<<producerNum<< ": "<< prime << " time: "<< clock()<<endl; 
    char buf[5];
    snprintf(buf, sizeof(buf), "%d", prime);
    for(int i=0; i<5; i++){
      *s++ = buf[i];
    }
    *s++ = DELIM;
    *s = '\0';
  }
}

void consumer(int consumerNum)
{
  int shmid;
  key_t key;
  char *shm, *s;
  char c;
  // key specified by the producer
  key = 5678;

  // get shared segment
  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  // attach shared segment to the data region
  if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
    exit(1);
  }

  // Consume
  while(true){
    int len = 0;
    srand(time(NULL));
    int r = rand()%5;
    sleep(r);
    s = shm;
    if(isEmpty(s))
      continue;
    cout << "Consumer "<< consumerNum<< ": ";
    for (s= shm; *s != DELIM; s++)
      putchar(*s);
    s = shm;
    int i;
    for(i=0; *(s+i+WORD_LENGTH) != '\0'; i++){
      c = *(s+i+WORD_LENGTH);
      *(s+i) = c;
    }
    *(s+i) = '\0';
    cout<< " time: "<< clock()<<endl;
  }
}

int main(){
  int np, nc;
  cout << "Enter number of producers and consumers respectively" << endl;
  cin >> np >> nc;
  vector<int> pids;

  char c;
  int shmid;
  key_t key;
  char *shm, *s;
  key = 5678;

  // create shared memory segment
  shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);
  if (shmid<0) {
    perror("shmget");
    exit(1);
  }

  // attach shared memory segment
  if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
    exit(1);
  }
  s = shm;
  *s = '\0';
  for(int i=0; i < np; i++){
    int pid = fork();
    if(pid==0){
      producer(i);
      exit(0);
    } else{
      pids.push_back(pid);
    }
  }
  for(int i=0; i < nc; i++){
    int pid = fork();
    if(pid == 0){
      consumer(i);
      exit(0);
    } else{
      pids.push_back(pid);
    }
  }
  sleep(30);
  int temp;
  for(int i= 0; i < pids.size(); i++){
    temp = kill(pids[i], SIGTERM);
  }
  return 0;
}
