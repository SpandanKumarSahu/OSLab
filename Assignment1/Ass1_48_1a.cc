#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM 100

int comp (const void * elem1, const void * elem2) 
{
  int f = *((int*)elem1);
  int s = *((int*)elem2);
  if (f > s) return  1;
  if (f < s) return -1;
  return 0;
}

/* Read characters from the pipe and echo them to stdout. */
void read_from_pipe(int file1, int file2, int file3){  
  int a, b, c;
  FILE *stream1, *stream2, *stream3;
  stream1 = fdopen(file1, "r");
  stream2 = fdopen(file2, "r");
  stream3 = fdopen(file3, "r");
  for(int i = 0; i < NUM; i++){
    fscanf(stream1, "%d", &a);
    fscanf(stream2, "%d", &b);
    fscanf(stream3, "%d", &c);
    printf("%d\n", (a<b)?((a<c)?a:c):((b<c)?b:c));
  }
  fclose(stream1);
  fclose(stream2);
  fclose(stream3);
}

/* Write some random text to the pipe. */
void write_to_pipe(int file){
  srand(file);
  int ar[NUM];
  for(int i = 0; i<NUM; i++){
    ar[i] = rand()%10000;
  }
  qsort(ar, NUM, sizeof(*ar), comp);
  FILE *stream;
  stream = fdopen(file, "w");
  for(int i = 0; i<NUM; i++)
    fprintf(stream, "%d\n", ar[i]);
  fclose(stream);
}

int main(void){
  pid_t pid;
  int mypipe1[2];
  int mypipe2[2];
  int mypipe3[2];

  /* Create the pipe.*/
  if (pipe(mypipe1)){
    fprintf(stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }
  /* Create the 1st child process. */
  pid = fork();
  if (pid == (pid_t) 0){
    /* This is the child process. Close other end first. */
    close(mypipe1[0]);
    write_to_pipe(mypipe1[1]);
    return EXIT_SUCCESS;
  } else if (pid < (pid_t) 0){
    /* The fork failed. */
    fprintf(stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  } else{
    /* Create the 2nd child process. */
    if (pipe(mypipe2)){
      fprintf(stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
    }
    pid = fork();
    if (pid == (pid_t) 0){
      /* This is the child process. Close other end first. */
      close(mypipe2[0]);            
      write_to_pipe(mypipe2[1]);
      return EXIT_SUCCESS;
    } else if (pid < (pid_t) 0){
      /* The fork failed. */
      fprintf(stderr, "Fork failed.\n");
      return EXIT_FAILURE;
    } else{
      /* This is the parent process. Close other end first. */
      if (pipe(mypipe3)){
	fprintf(stderr, "Pipe failed.\n");
	return EXIT_FAILURE;
      }
      pid = fork();
      if (pid == (pid_t) 0){
	/* This is the child process. Close other end first. */
	close(mypipe1[1]);
	close(mypipe2[1]);
	close(mypipe3[1]);
	read_from_pipe(mypipe1[0], mypipe2[0], mypipe3[0]);
	return EXIT_SUCCESS;
      } else if (pid < (pid_t) 0){
	/* The fork failed. */
	fprintf(stderr, "Fork failed.\n");
	return EXIT_FAILURE;
      } else{
	/* This is the parent process. Close other end first. */
	close(mypipe3[0]);
	write_to_pipe(mypipe3[1]);
	return EXIT_SUCCESS;
      }
    }
  }
}
