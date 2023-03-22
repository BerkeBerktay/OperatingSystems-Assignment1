#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>


void child_task(int id, char *commandname, int *pipefd) {
    double timepassed;
    struct timeval begin, terminate;
    gettimeofday(&begin, NULL);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        execlp(commandname, commandname, NULL);
        perror(commandname);
        exit(EXIT_FAILURE);
    }else{
        wait(NULL);
        gettimeofday(&terminate, NULL);
        timepassed = (((terminate.tv_sec - begin.tv_sec) * 1000.0) + ((terminate.tv_usec - begin.tv_usec) / 1000.0));
        write(pipefd[1], &timepassed, sizeof(timepassed));
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }

}

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    char *commandname = argv[2];

    int pipefd[n][2];

    for(int i = 0; i < n; i++){
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            close(pipefd[i][0]);
            child_task(i+1, commandname, pipefd[i]);
            exit(0);
        }else{
            close(pipefd[i][1]);
        }
    }


    double timelist[n];

    for (int i = 0; i < n; i++) {
        read(pipefd[i][0], &timelist[i], sizeof(timelist[i]));
        close(pipefd[i][0]);
    }

    double sum;
    double max = INFINITY;
    double min = 0;

    for (int i = 1; i < n; i++) {
        sum = sum + timelist[i];
        if(timelist[i] <= INFINITY){
            min = timelist[i];
        }
        if(timelist[i] >= 0){
            max = timelist[i];
        }
    }
    double average = sum / n;

    for (int i = 0; i < n; i++) {
        printf("Child %d Executed in %.2f millis\n", i+1, timelist[i]);
    }
    printf("Max: %.2f millis\n", max);
    printf("Min: %.2f millis\n", min);
    printf("Average: %.2f millis\n", average);

    exit(0);
}