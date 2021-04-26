#include <pthread.h> //thread functions
#include <stdbool.h> //bool
#include <stdio.h>
#include <stdlib.h> //rand()
#include <unistd.h> //usleep()

#include "cmd_parser.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int current_thread = 0; // identificador da thread atual (I think, not sure)

// implementação rafada do waiting for threads/ Ze: Acho que isto vai ser inutil
int working_threads = 0;
//-------------------------------------------

void *thread_processing(void *ptr) {
  working_threads++;
  pthread_mutex_lock(&lock);
  current_thread++;
  int task = rand() % 9 + 1;
  // testing waiting for threads
  sleep(1);
  printf("Thread %d: task %d\n", current_thread, task);
  //--------------------------
  pthread_mutex_unlock(&lock);
  working_threads--;
  return NULL;
}

int main(int argc, char *const argv[]) {

  int nsecs;
  char *fifoname;
  if (cmdParser(argc, argv, &nsecs, &fifoname) != 0) {
    return 1;
  }

  // debug
  printf("nsecs = %d, fifoname = %s\n\n", nsecs, fifoname);
  //-----

  bool close = false; // alterar o valor quando request negativo (MUDAR PARA
                      // GLOBAL LATER probably)
                      // Ze: Request negativo? Tipo o fifo publica fecha? Podemos usar o SIGPIPE - lançado quando um write falha

  // Abrir fifo publica

  // Setup detached threads

  // Setup timers

  // para testar criar 10 pedidos
  int c = 1;

  while (!close /*&& nsecs < getTime()*/) {

    int rand_numb = (rand() % 1000) * 1000; // random milissecond number, from 0 ms to 1 sec
    usleep(rand_numb);

    pthread_t tid;

    // debug
    printf("created %d\n", c);
    //-----

    //Ze: Mudar para detached threads
    pthread_create(&tid, NULL, thread_processing, (void *)&rand_numb);

    // para testar criar 10 pedidos
    c++;
    if (c > 10)
      break;
    //----------------------------
  }
  

  // Destruir o setup das detached threads
  return 0; // terminar com pthread_exit
}
// Ze: Aqui a ideia é a main thread morrer, e as detached threads matam se e vão à sua vida, sem ser preciso fazer cleanup
