#include <pthread.h> //thread functions
#include <stdbool.h> //bool
#include <stdio.h>
#include <stdlib.h> //rand()
#include <unistd.h> //usleep()
#include <time.h>   //clock functs
#include <sys/types.h>  // CLOCK_REALTIME
#include <sys/stat.h> // open()
#include <fcntl.h> // open()
#include <errno.h> // perror()



#include "timer.h"
#include "cmd_parser.h"


void *thread_processing(void *taskId) {

  int t = rand() % 9 + 1;
  //pid_t pid = getpid();
  //pthread_t tid = pthread_self();
  // testing waiting for threads (main operation, mudar mais tarde)
  sleep(1);
  printf("TaskId: %lu; load: %d;\n", *(unsigned long*)taskId, t);
  //--------------------------
  return NULL;
}

int main(int argc, char *const argv[]) {

  //set start time in time.c
  // Ze: I guess it shouldn't be here
  set_start();

  int nsecs;
  char *fifoname;
  if (cmdParser(argc, argv, &nsecs, &fifoname) != 0) {
    return 1;
  }

  // debug
  printf("nsecs = %d, fifoname = %s\n\n", nsecs, fifoname);
  //-----

  // Open public fifo
  /*
  errno = 0;
  int pubFifoFD = open(fifoname, O_WRONLY);
  if(pubFifoFD == -1){
    perror("Public error");
    return 1;
  }
  */

  // debug
  // para testar criar 10 pedidos
  int c = 0;
  // ----------

  // Setup detached threads
  pthread_attr_t detatched;
  errno = 0;
  if(pthread_attr_init(&detatched) != 0){
    perror("Detached threads setup (init)");
    return 1;
  }
  if(pthread_attr_setdetachstate(&detatched, PTHREAD_CREATE_DETACHED) != 0){
    perror("Detached threads setup (setdetachstate)");
    return 1;
  }

  pthread_t tid;

  // TaskId's
  unsigned long taskId = 0;
  
  while (nsecs > get_elapsed()) {  // Bool para guardar se a resposta de a alguma thread foi closd
    // Pseudo random interval between thread creation
    int rand_numb = (rand() % 1000) * 1000; // random milissecond number, from 0 ms to 1 sec
    usleep(rand_numb);
    
    
    printf("created %d\n", c);
    pthread_create(&tid, &detatched, thread_processing, (void *)&taskId);
    taskId++;

    // debug
    // para testar criar 10 pedidos
    c++;
    if (c >= 10)
      break;
    // ----------------------------
  } 
  printf("Main thread termination\n");

  // Destruir o setup das detached threads
  pthread_attr_destroy(&detatched);

  pthread_exit(0); // terminar com pthread_exit
}
