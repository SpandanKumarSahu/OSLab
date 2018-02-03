#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
#include <math.h>
#include <queue>
using namespace std;

int merge(double arr[],int l,int m,int h)
{
  int arr1[10],arr2[10];  // Two temporary arrays to
  //hold the two arrays to be merged
  int n1,n2,i,j,k;
  n1=m-l+1;
  n2=h-m;

  for(i=0;i<n1;i++)
    arr1[i]=arr[l+i];
  for(j=0;j<n2;j++)
    arr2[j]=arr[m+j+1];

  arr1[i]=9999;  // To mark the end of each temporary array
  arr2[j]=9999;

  i=0;j=0;
  for(k=l;k<=h;k++)  //process of combining two sorted arrays
    {
      if(arr1[i]<=arr2[j])
	arr[k]=arr1[i++];
      else
	arr[k]=arr2[j++];
    }
  
  return 0;
}

int merge_sort(double arr[],int low,int high)
{
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

double FCFS(double arrival[],double cpu[],int n)
{
  double ans;
  int sum=0;
  int tab[100][2];
  tab[0][0]=0;
  tab[0][1]=cpu[0];
  for(int i=1;i<n;i++)
    {	
      if(arrival[i]>tab[i-1][1])
	{
	  tab[i][0]=arrival[i];
	}
      else
	{
	  tab[i][0]=tab[i-1][1];
	}	
      tab[i][1]=tab[i][0]+cpu[i];
    }		
  for(int i=0;i<n;i++)
    {
      sum+=tab[i][1]-arrival[i];
    }
  ans=(double)sum/n;
  return ans;
}

double RR(double arrival[],double cpu[],int n)
{
  double sum=0;
  int j=0,job=1,tmp;
  double finish[100];
  queue <int> myq;
  int quantum=1;
  myq.push(0);
  int time=0;
  while(!myq.empty())
    {
      //cerr << "~" << myq.size() << endl;
      while(arrival[job]<=time)
	{
	  if(job==n-1)
	    break;
	  myq.push(job);
	  job++;
	}
      tmp=myq.front();
      if(cpu[tmp]<quantum)
	{
	  time-=cpu[tmp]-quantum;
	  finish[tmp]=time;
	  myq.pop();
	}
      else if(cpu[tmp]>quantum)
	{
	  cpu[tmp]-=quantum;
	  time+=quantum;
	  myq.pop();
	  myq.push(tmp);
	}
      else
	{
	  finish[tmp]=time;
	  time+=quantum;
	  myq.pop();	
	}
    }
  for(int i=0;i<n;i++)
    {
      sum+=finish[i]-arrival[i];
    }
  return (double)sum/n;
}

int main()
{
  int n,tmp;
  cin>>n;
  double arrival[100],cpu[100];
  srand(time(NULL));
  arrival[0]=0;
  for(int i=1;i<n;i++)
    {
      arrival[i]=((double)rand()/RAND_MAX);
      //cout<<arrival[i]<<endl;
      arrival[i]=(int)fmod(-(log(arrival[i])/.004),10.0);
      //cout<<arrival[i]<<endl;
    }
  cout<<endl;
  sort(arrival,arrival+n);
  for(int i=0;i<n;i++)
    {
      cout<<arrival[i]<<endl;
    }
  cout<<endl;
  for(int i=0;i<n;i++)
    {
      cpu[i]=((double)rand()/RAND_MAX);
      //cout<<arrival[i]<<endl;
      cpu[i]=(int)fmod(-(log(cpu[i])/.004),19.0)+1;
      cout<<cpu[i]<<endl;
    }
  //cout<<"qwe";
  cout<<"FCFS(turnaround time)="<<FCFS(arrival,cpu,n)<<endl;
  cout<<"RR(turnaround time)="<<RR(arrival,cpu,n)<<endl;
  return 0;
}
