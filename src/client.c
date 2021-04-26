#include <pthread.h> //thread functions
#include <stdbool.h> //bool
#include <stdio.h>
#include <stdlib.h> //rand()
#include <unistd.h> //usleep()
#include <time.h>   //clock functs
#include <sys/types.h>  // CLOCK_REALTIME mkfifo()
#include <sys/stat.h> // open() mkfifo()
#include <fcntl.h> // open()
#include <errno.h> // perror()
#include <string.h> // snprintf() strcat()

#include "timer.h"
#include "cmd_parser.h"
#include "common.h" //message


#define FIFONAME_LEN 1000


int pubFifoFD = -1;


void *cThreadFunc(void *taskId) {
  // Generate task load
  int load = rand() % 9 + 1;
  
  // Set message struct
  Message message;
	message.rid = *(int*)(taskId);
	message.tskload = load;
	message.pid = getpid();
	message.tid = pthread_self();
	message.tskres = -1;

  // Assemble fifoname
  char privatefifoname[FIFONAME_LEN];
  snprintf(privatefifoname, FIFONAME_LEN, "/tmp/%d.%ld", message.pid, message.tid);

  // Create fifo
  errno = 0;
	if (mkfifo(privatefifoname, 0777) == -1) {
		if (errno != EEXIST) {
			perror("Error creating private Fifo");
      pthread_exit(0);
		}
	}

  // Open private fifo
  errno = 0;
	int privFifoFD = open(privatefifoname, O_RDONLY);
	if (privFifoFD == -1) {
		perror("Error opening private fifo");
    pthread_exit(0);
	}

  printf("%ld ; %d ; %d ; %d;  %ld; %d; IWANT\n", get_time(), message.rid, message.tskload, message.pid, message.tid, message.tskres);
  if (write(pubFifoFD, &message, sizeof(Message)) == -1) {
		perror("Error writing to public fifo");
    pthread_exit(0);
	}

  sleep(1);
  printf("TaskId: %d; load: %d;\n", *(int*)(taskId), load);
  //--------------------------
  pthread_exit(0);
}

int main(int argc, char *const argv[]) {

  //set start time in time.c
  // Ze: I guess it shouldn't be here
  set_start();

  int nsecs;
  char *fifoname;
  if (cmdParser(argc, argv, &nsecs, &fifoname) != 0) {
    exit(EXIT_FAILURE);
  }

  // debug
  printf("nsecs = %d, fifoname = %s\n\n", nsecs, fifoname);
  //-----

  // Open public fifo
  
  errno = 0;
  pubFifoFD = open(fifoname, O_WRONLY);
  if(pubFifoFD == -1){
    perror("Error opening public fifo");
    exit(EXIT_FAILURE);
  }
  

  // Setup detached threads
  pthread_attr_t detatched;
  errno = 0;
  if(pthread_attr_init(&detatched) != 0){
    perror("Error in detached threads setup (init)");
    exit(EXIT_FAILURE);
  }
  if(pthread_attr_setdetachstate(&detatched, PTHREAD_CREATE_DETACHED) != 0){
    perror("Error in detached threads setup (setdetachstate)");
    exit(EXIT_FAILURE);
  }

  pthread_t tid;

  // TaskId's
  int taskId = 0;
  
  while (nsecs > get_elapsed()) {  // Bool para guardar se a resposta de a alguma thread foi closd
    // Pseudo random interval between thread creation
    int rand_numb = (rand() % 1000) * 1000; // random milissecond number, from 0 ms to 1 sec
    errno = 0;
    if(usleep(rand_numb) == -1){
      perror("Error usleep");
      exit(EXIT_FAILURE);
    }
    
    
    printf("created %d\n", taskId);
    errno = 0;
    if(pthread_create(&tid, &detatched, cThreadFunc, (void *)&taskId) != 0){
      perror("Error creating threads");
      exit(EXIT_FAILURE);
    }

    taskId++;

    // debug
    // para testar criar 10 pedidos
    if (taskId >= 10)
      break;
    // ----------------------------
  } 
  printf("Main thread termination\n");

  // Destruir o setup das detached threads
  pthread_attr_destroy(&detatched);

  pthread_exit(0); // terminar com pthread_exit
}
