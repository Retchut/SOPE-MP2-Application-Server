#include <pthread.h> //thread functions
#include <stdbool.h> //bool
#include <stdio.h>
#include <stdlib.h> //rand()
#include <unistd.h> //usleep()
#include <time.h>   //clock functs
#include <sys/types.h>  //CLOCK_REALTIME

#include "utils.h"
#include "cmd_parser.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int current_thread = 0; // identificador da thread atual

void *thread_processing(void *ptr) {

  //increment id
  pthread_mutex_lock(&lock);
  int id = current_thread;
  current_thread++;
  pthread_mutex_unlock(&lock);

  int task = rand() % 9 + 1;
  // testing waiting for threads (main operation, mudar mais tarde)
  sleep(1);
  printf("Thread %d: task %d\n", id, task);
  //--------------------------
  return NULL;
}

int main(int argc, char *const argv[]) {

  //set start time in time.c
  set_start();

  int nsecs;
  char *fifoname;
  if (cmdParser(argc, argv, &nsecs, &fifoname) != 0) {
    return 1;
  }

  // debug
  printf("nsecs = %d, fifoname = %s\n\n", nsecs, fifoname);
  //-----

  // Abrir fifo publica
  //open_fifo();

  // para testar criar 10 pedidos
  int c = 0;

  // Setup detached threads
  pthread_attr_t detatched;
  pthread_attr_init(&detatched);
  pthread_attr_setdetachstate(&detatched, PTHREAD_CREATE_DETACHED);
  
  while (nsecs > get_elapsed()) {

    int rand_numb = (rand() % 1000) * 1000; // random milissecond number, from 0 ms to 1 sec
    usleep(rand_numb);

    pthread_t tid;

    printf("created %d\n", c);
    //Ze: Mudar para detached threads
    pthread_create(&tid, &detatched, thread_processing, (void *)&rand_numb);


    // para testar criar 10 pedidos
    c++;
    if (c >= 10)
      break;
    //----------------------------
  } 
  printf("out\n");

  // Destruir o setup das detached threads
  pthread_attr_destroy(&detatched);

  pthread_exit(0); // terminar com pthread_exit
}
