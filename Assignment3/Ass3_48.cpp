#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
#include <math.h>
#include <queue>
using namespace std;

int merge(double arr[],int l,int m,int h){
  int arr1[10],arr2[10];  // Two temporary arrays to
  //hold the two arrays to be merged
  int n1,n2,i,j,k;
  n1=m-l+1;
  n2=h-m;

  for(i=0;i<n1;i++)
    arr1[i]=arr[l+i];
  for(j=0;j<n2;j++)
    arr2[j]=arr[m+j+1];

  // To mark the end of each temporary array
  arr1[i]=9999;
  arr2[j]=9999;

  //process of combining two sorted arrays
  i=0;j=0;
  for(k=l;k<=h;k++){
    if(arr1[i]<=arr2[j])
      arr[k]=arr1[i++];
    else
      arr[k]=arr2[j++];
  }
  return 0;
}

int merge_sort(double arr[],int low,int high){
  int mid;
  if(low<high)
    {
      mid=(low+high)/2;
      // Divide and Conquer
      merge_sort(arr,low,mid);
      merge_sort(arr,mid+1,high);
      // Combine
      merge(arr,low,mid,high);
    }  
  return 0;
}

double FCFS(double arrival[],double cpu[],int n){
  double ans;
  int sum=0;
  double tab[100][2];
  tab[0][0]=0;
  tab[0][1]=cpu[0];
  for(int i=1;i<n;i++){
    if(arrival[i]>tab[i-1][1])
      tab[i][0]=arrival[i];
    else
      tab[i][0]=tab[i-1][1];
    tab[i][1]=tab[i][0]+cpu[i];
  }
  for(int i=0;i<n;i++)
    sum+=tab[i][1]-arrival[i];
  ans=((double)sum)/n;
  return ans;
}

bool isFinished(vector<double> v){
  for(int i = 0; i<v.size(); i++)
    if(v[i] != 0.0)
      return false;
  return true;
}

double SJF(double arrival[],double cpu[],int n){
  double finish_time[n];
  memset(finish_time, 0, n);
  vector<double> v_arrivals(arrival, arrival+n);
  vector<double> v_cpu;
  for(int i=0; i<n; i++){
    v_cpu.push_back(cpu[i]);
  }
  double cur_time = 0.0;
  int min_index = 0;
  int next_arrival_index = 1;
  double diff = 0.0;

  while(!isFinished(v_cpu) && next_arrival_index < n){
    for(int i = 0; i<next_arrival_index; i++){
      if(v_cpu[i] != 0.0){
	if(v_cpu[min_index] > v_cpu[i])
	  min_index = i;
      }
    }
    diff = min((v_arrivals[next_arrival_index] - v_arrivals[min_index]),
	       v_cpu[min_index]);
    v_cpu[min_index] -= diff;
    finish_time[min_index] = cur_time + diff;
    cur_time = v_arrivals[next_arrival_index];
    v_arrivals[min_index] = cur_time;
    next_arrival_index += 1;
  }

  while(!isFinished(v_cpu)){
    for(int i = 0; i<n; i++){
      if((v_cpu[min_index] > v_cpu[i] && v_cpu[i] != 0.0) || v_cpu[min_index] == 0.0)
	min_index = i;
    }
    diff = v_cpu[min_index];
    v_cpu[min_index] -= diff;
    cur_time += diff;
    finish_time[min_index] = cur_time;
    v_arrivals[min_index] = cur_time;
  }
  diff = 0.0;
  for(int i = 0; i<n; i++)
    diff += (finish_time[i]-arrival[i]);
  return diff/n;
}

double RR(double arrival[],double in_cpu[],int n, int quantum=1){
  double sum=0;
  int j=0,job=1,tmp;
  double finish[100];
  queue <int> myq;
  double cpu[n];
  for(int i=0; i<n; i++)
    cpu[i] = in_cpu[i];
  myq.push(0);
  int time=0;
  while(!myq.empty()){
    while(arrival[job]<=time){
      if(job==n-1)
	break;
      myq.push(job);
      job++;
    }
    tmp=myq.front();
    if(cpu[tmp]<quantum){
      time-=cpu[tmp]-quantum;
      finish[tmp]=time;
      myq.pop();
    }
    else if(cpu[tmp]>quantum){
      cpu[tmp]-=quantum;
      time+=quantum;
      myq.pop();
      myq.push(tmp);
    }
    else{
      finish[tmp]=time;
      time+=quantum;
      myq.pop();
    }
  }
  for(int i=0;i<n;i++)
    sum+=(finish[i]-arrival[i]);
  return ((double)sum)/n;
}

int main()
{
  int n,tmp;
  int ar_n[3] = {10, 50, 100};
  for(int x=0; x<3; x++){
    n = ar_n[x];
    double arrival[100],cpu[100];
    srand(time(NULL));
    arrival[0]=0;
    for(int i=1;i<n;i++){
      arrival[i]=((double)rand()/RAND_MAX);
      arrival[i]=(double)fmod(-(log(arrival[i])/.004),10.0);
    }
    sort(arrival,arrival+n);
    cout<< "Arrival Times:"<<endl;
    for(int i=0;i<n;i++)
      cout<<arrival[i]<<" ";
      cout<< "\nCPU Bursts:"<<endl;
    for(int i=0;i<n;i++){
      cpu[i]=((double)rand()/RAND_MAX);
      cpu[i]=(double)fmod(-(log(cpu[i])/.004),19.0)+1;
      cout<<cpu[i]<<" ";
    }
    cout<<endl;
    cout<<"FCFS:\t"<<FCFS(arrival,cpu,n)<<endl;
    cout<<"PSJF:\t"<<SJF(arrival,cpu,n)<<endl;
    cout<<"RR01:\t"<<RR(arrival,cpu,n)<<endl;
    cout<<"RR02:\t"<<RR(arrival,cpu,n, 2)<<endl;
    cout<<"RR05:\t"<<RR(arrival,cpu,n, 5)<<endl;
  }
  return 0;
}
