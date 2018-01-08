#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Read characters from the pipe and echo them to stdout. */
void read_from_pipe(int file){
    FILE *stream;
    int c;
    stream = fdopen(file, "r");
    while ((c = fgetc(stream)) != EOF)
        putchar (c);
    fclose (stream);
}

/* Write some random text to the pipe. */
void write_to_pipe(int file){
    FILE *stream;
    stream = fdopen(file, "w");
    fprintf (stream, "hello, world!\n");
    fclose (stream);
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
                read_from_pipe(mypipe1[0]);
                read_from_pipe(mypipe2[0]);
                read_from_pipe(mypipe3[0]);
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
