#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define MAX_THREADS 1000

#define READY 0
#define RUNNING 1
#define FINISHED 2

/* Status maintains the status of the threads.
 * 0: Blocked/Waiting
 * 1: Running
 * 2: Finished
*/

queue<int> queue_ready;
vector<int> status;
vector<pthread_t> threads;
int queue_len = 0;
int num_threads;


void signal_handler(int signum){
  if (signum == SIGUSR2) {
    /* wake up */
  } else if (signum == SIGUSR1) {
    /* for sleep */
    sigset_t myset;
    sigsuspend(&myset);
  }
}

void* worker(void* arg){
  int arr[1000];
  srand(*((int*)arg));
  for(int i=0;i<1000;i++)
    arr[i]=rand()%10000;
  sort(arr,arr+100);
  sleep(rand()%10+1);

  int callee = (int) pthread_self();
  int pos = 0;
  for(; pos<num_threads; pos++){
    if(callee == threads[pos])
      break;
  }
  status[pos] = FINISHED;
  pthread_exit(0);
}

void* round_robin_scheduler(void *arg){
  int quantum = 1;
  int i=queue_ready.front();
  queue_ready.pop();
  status[i] = RUNNING;
  pthread_kill(threads[i], SIGUSR2);
  sleep(quantum);

  while(true){
    if(status[i] != FINISHED){
      pthread_kill(threads[i%num_threads], SIGUSR1);
      queue_ready.push(i);
      status[i] = READY;
    } else {
      queue_len--;
    }
    if(!queue_ready.empty()){
      i = queue_ready.front();
      queue_ready.pop();
      status[i] = RUNNING;
      pthread_kill(threads[i], SIGUSR2);
      sleep(quantum);
    } else {
      break;
    }
  }
  pthread_exit(0);
}

void* reporter(void *arg){
  vector<int> ss(status), fin(num_threads, FINISHED);
  while(ss != fin) {
    if (ss == status) {
      /* if status if equal to the previous status do nothing*/
    }
    else{
      /* if status is not equat to previous or it has been changed then 
	 print the record*/
      cout << "\n\ncurrent status of threads\n";
      for (int i = 0; i < num_threads; i++){
	switch(status[i]){
	case 0:
	  cout << "READY";
	  break;
	case 1:
	  cout << "RUNNING";
	  break;
	case 2:
	  cout << "FINISHED";
	  break;
	default:
	  break;
	}
	cout << threads[i] << endl;
      }
      ss = status;
    }
    pthread_exit(0);
  }
}


int main(){
  if (signal(SIGUSR2, signal_handler) == SIG_ERR)
    cout << "\nCan't catch SIGUSR2\n";
  if (signal(SIGUSR1, signal_handler) == SIG_ERR)
    cout << "\nCan't catch SIGUSR1\n";

  cout<<"Enter no. of threads"<<endl;
  cin >> num_threads;

  int seed_ar[num_threads];
  int temp;
  pthread_t scheduler_id, reporter_id;
  pthread_attr_t attr;
  srand(time(NULL));
  pthread_attr_init(&attr);

  status.resize(num_threads);
  threads.resize(num_threads);

  for(int i=0; i<num_threads; i++){
    seed_ar[i]=rand()%10000;
    temp = pthread_create(&threads[i], NULL, worker, (void*)&seed_ar[i]);
    if(temp){
      cout<<"thread cannot be created";
      exit(EXIT_FAILURE);
    }
    //Initially stop the process
    pthread_kill(threads[i], SIGUSR1);
    queue_len++;
    status[i] = READY;
  }

  if(pthread_create(&scheduler_id, NULL, round_robin_scheduler, NULL)){
    cout << "Scheduler thread cannot be created"<<endl;
    exit(EXIT_FAILURE);
  }

  if(pthread_create(&reporter_id, NULL, reporter, NULL)){
    cout << "Scheduler thread cannot be created"<<endl;
    exit(EXIT_FAILURE);
  }

  for(int i=0; i<num_threads; i++)    
    pthread_join(threads[i],NULL);
  pthread_join(scheduler_id, NULL);
  pthread_join(reporter_id, NULL);
  return 0;
}
