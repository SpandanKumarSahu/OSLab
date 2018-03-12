#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
using namespace std;

#define READY 0
#define RUNNING 1
#define TERMINATED 2
#define MAX_THREADS 1000

queue<int> readyQ;
map<pthread_t, int> mp;
int num_threads;
vector<int> status;
vector<pthread_t> threads;


void sig_handler(int signum){
  if (signum == SIGUSR2) {
    /* wake up */
  } else if (signum == SIGUSR1) {
    /* for sleep */
    sigset_t myset;
    sigsuspend(&myset);
  }
}

void* worker(void *arg){
  int arr[10000];
  for(int i=0;i<10000;i++)
    arr[i]=rand()%10000;
  sort(arr,arr+100);

  // Sleeping otherwise it gets terminated frequently
  sleep(rand()%10+1);
  status[mp[pthread_self()]] = TERMINATED;
  pthread_exit(0);
}


void* scheduler(void *arg){
  int cur_id = -1;
  int temp;
  int quantum = 1;

  while(true) {
    if(readyQ.empty())
      break;		
    if (status[cur_id] != TERMINATED ) {
      if(cur_id != -1){
	readyQ.push(cur_id);
	status[cur_id] = READY;
	pthread_kill(threads[cur_id], SIGUSR1);
      }
    }
    if (not readyQ.empty()) {
    }
    else
      break;
    cur_id = readyQ.front();
    readyQ.pop();

    status[cur_id] = RUNNING;
    pthread_kill(threads[cur_id], SIGUSR2);
    sleep(quantum);
  }
  pthread_exit(0);
}

void* reporter(void *arg){
  vector<int> ss(status), fin(num_threads, TERMINATED);
  while(ss != fin){
    if (ss == status) {
      /* if status if equal to the previous status do nothing*/
    }
    else{
      cout << "\nCurrent status of threads\n"<< endl;
      for (int i = 0; i < num_threads; i=i+1){
	cout << threads[i] << " is ";
	switch(status[i]){
	case 0: cout << "READY";
	  break;
	case 1: cout << "RUNNING";
	  break;
	case 2: cout << "TERMINATED";
	  break;
	default:
	  break;
	}
	cout << endl;
      }
      ss = status;
    }
  }
  pthread_exit(0);
}

int main(int argc, char const *argv[]){
  int j;
  pthread_t scheduler_tid, reporter_tid;
  pthread_attr_t attr;

  // Install Signal Handler
  if (signal(SIGUSR2, sig_handler) == SIG_ERR)
    cout << "\nCan't catch SIGUSR2\n";
  if (signal(SIGUSR1, sig_handler) == SIG_ERR)
    cout << "\nCan't catch SIGUSR1\n";

  printf("Enter the number of workers : ");
  cin >> num_threads;

  status.resize(num_threads);
  threads.resize(num_threads);

  // set default attrs.
  pthread_attr_init(&attr);

  // create num_threads workers
  j=0;
  while(j < num_threads){
    pthread_create(&threads[j], &attr, worker, NULL);
    mp[threads[j]] = j;
    readyQ.push(j);
    status[j] = READY;
    
    //Initially stop all threads
    pthread_kill(threads[j], SIGUSR1);
    j++;	
  }
  pthread_create(&reporter_tid, &attr, reporter, NULL);
  pthread_create(&scheduler_tid, &attr, scheduler, NULL);

  j=num_threads-1;
  while(j >= 0){
      pthread_join(threads[j], NULL);
      j--;
  }
  pthread_join(reporter_tid, NULL);	
  pthread_join(scheduler_tid, NULL);

  return 0;
}
