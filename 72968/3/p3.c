#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>


int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  int x = atoi(argv[2]);

  char buf[1000];
  int array[1000];

  int shm_id = shmget(IPC_PRIVATE, 1000 * sizeof(int), IPC_CREAT | 0666);
  if (shm_id < 0) {
        perror("shmget");
        return 1;

  }

  int *list = (int*) shmat(shm_id, NULL, 0);
  if (list == (int*) -1) {
      perror("shmat");
    return 1;
  }

  int listindex = 0;
  while (fgets(buf, 1000, stdin) != NULL) {
    array[listindex++] = atoi(buf);

  }

  int division = listindex / n;
  int endbatch = division + (listindex % n);




  pid_t *pids = (pid_t*) malloc(n * sizeof(pid_t));
  if (pids == NULL) {
    perror("malloc");
    return 1;
  }

  for (int i = 0; i < n; i++) {
    pids[i] = fork();
    if (pids[i] == -1) {
        perror("fork");
        return 1;
    }
    if (pids[i] == 0) {

        int begin = i * division;
        int terminate;

        if(i == n - 1){
         terminate = begin + endbatch;
        }
        else{
         terminate = begin + division;
        }


        for (int j = begin; j < terminate; j++) {
            if (array[j] == x) {
                printf("%d found in rank %d\n", x, j);
                exit(0);
            }
        }
        exit(1);
    }
  }


  int state;
  pid_t cpid;
  while ((cpid = wait(&state)) > 0) {
      if (state ==0){
         for (int i = 0; i < n; i++) {
               if (pids[i] != cpid) {
                    kill(pids[i], SIGTERM);
               }
         }
         shmdt(list);
         shmctl(shm_id, IPC_RMID, NULL);
         return 0;
      }
  }




  shmdt(list);
  shmctl(shm_id, IPC_RMID, NULL);
  printf("%d not in the provided list\n", x);
  return 1;

}