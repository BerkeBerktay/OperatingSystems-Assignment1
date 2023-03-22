#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid != 0) {
        kill(pid, SIGSEGV);
        printf("you killed the man, but not the idea");
    }
    sleep(15);
    wait(NULL);
    exit(0);
}