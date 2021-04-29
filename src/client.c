// COPYRIGHT 2021 Flávio Lobo Vaz, José Costa, Mário Travassos, Tomás Fidalgo

#include <errno.h>   // perror()
#include <fcntl.h>   // open()
#include <pthread.h>  // thread functions
#include <stdbool.h>  // bool
#include <stdio.h>
#include <stdlib.h>      // rand_r() atexit()
#include <string.h>      // snprintf() strcat()
#include <sys/inotify.h>  // inotify funcs
#include <sys/stat.h>    // open() mkfifo()
#include <sys/types.h>   // CLOCK_REALTIME mkfifo()
#include <time.h>        // clock functs
#include <unistd.h>      // usleep()

#include "./cmd_parser.h"
#include "./common.h"  // message
#include "./timer.h"

#define FIFONAME_LEN 1000


static volatile int pubFifoFD = -1;
static volatile bool serverOpen = true;

void cThreadFunc(void *taskId) {
  // Generate task load
  time_t seed = time(NULL);
  int load = rand_r((unsigned int*)(&seed)) % 9 + 1;

  // Set message struct
  Message message, recvdMessage;
  memset(&message, 0, sizeof(message));
  memset(&recvdMessage, 0, sizeof(message));
  message.rid = *(int *)(taskId);
  message.tskload = load;
  message.pid = getpid();
  message.tid = pthread_self();
  message.tskres = -1;

  // Assemble fifoname
  char privFifoName[FIFONAME_LEN];
  snprintf(privFifoName, FIFONAME_LEN, "/tmp/%d.%ld", message.pid, message.tid);

  // Create fifo
  if (mkfifo(privFifoName, 0777) == -1) {
    if (errno != EEXIST) {
      perror("Error creating private Fifo");
      pthread_exit(0);
    }
  }

  // Open private fifo
  int privFifoFD = open(privFifoName, O_RDONLY | O_NONBLOCK);
  if (privFifoFD == -1) {
    perror("Error opening private fifo");
    if (unlink(privFifoName) == -1) {
      perror("Error unlinking private fifo");
    }
    pthread_exit(0);
  }

  // Set select
  int dataReady = 0;
  fd_set rfds;
  struct timeval timeout;
  FD_ZERO(&rfds);
  FD_SET(privFifoFD, &rfds);
  timeout.tv_sec = getRemaining();
  timeout.tv_usec = 0;

  if (write(pubFifoFD, &message, sizeof(Message)) == -1) {
    perror("Error writing to public fifo");
    if (close(privFifoFD) == -1) {
      perror("Error closing private fifo");
    }
    if (unlink(privFifoName) == -1) {
      perror("Error unlinking private fifo");
    }
    pthread_exit(0);
  }

  printf("%ld ; %d ; %d ; %d ; %ld ; %d ; IWANT\n", getTime(), message.rid,
         message.tskload, message.pid, message.tid, message.tskres);

  // wait for response
  dataReady = select(privFifoFD + 1, &rfds, NULL, NULL, &timeout);

  if (dataReady == -1) {
    perror("Error in private fifo wait");
    if (close(privFifoFD) == -1) {
      perror("Error closing private fifo");
    }
    if (unlink(privFifoName) == -1) {
      perror("Error unlinking private fifo");
    }
    pthread_exit(0);
  } else if (dataReady == 0) {
    printf("%ld ; %d ; %d ; %d ; %ld ; %d ; GAVUP\n", getTime(), message.rid,
           message.tskload, message.pid, message.tid, message.tskres);

  } else {
    if (read(privFifoFD, &recvdMessage, sizeof(Message)) == -1) {
      perror("Error reading priv fifo");
      if (close(privFifoFD) == -1) {
        perror("Error closing private fifo");
      }
      if (unlink(privFifoName) == -1) {
        perror("Error unlinking private fifo");
      }
      pthread_exit(0);
    }

    // parse response
    if (recvdMessage.tskres == -1) {
      printf("%ld ; %d ; %d ; %d ; %ld ; %d ; CLOSD\n", getTime(),
             message.rid, message.tskload, message.pid,
             message.tid, recvdMessage.tskres);
      serverOpen = false;
    } else {
      printf("%ld ; %d ; %d ; %d ; %ld ; %d ; GOTRS\n", getTime(),
             message.rid, message.tskload, message.pid,
             message.tid, recvdMessage.tskres);
    }
  }
  if (close(privFifoFD) == -1) {
    perror("Error closing private fifo");
  }
  if (unlink(privFifoName) == -1) {
    perror("Error unlinking private fifo");
  }
  pthread_exit(0);
}

void closePubFifo(void) {
  if (close(pubFifoFD) == -1) {
    perror("Error closing public fifo");
  }
}

int main(int argc, char *const argv[]) {
  int nsecs;
  char *fifoname;
  if (cmdParser(argc, argv, &nsecs, &fifoname) != 0) {
    exit(EXIT_FAILURE);
  }

  // Setup detached threads
  pthread_attr_t detatched;

  if (pthread_attr_init(&detatched) != 0) {
    perror("Error in detached threads setup (init)");
    exit(EXIT_FAILURE);
  }
  if (pthread_attr_setdetachstate(&detatched, PTHREAD_CREATE_DETACHED) != 0) {
    perror("Error in detached threads setup (setdetachstate)");
    exit(EXIT_FAILURE);
  }

  pthread_t tid;

  // TaskIds
  int taskId = 0;

  // set start time in time.c
  setTimer(nsecs);

  // Open public fifo
  while (getRemaining() > 0 && pubFifoFD == -1) {
    pubFifoFD = open(fifoname, O_WRONLY | O_NONBLOCK);
    if (pubFifoFD == -1) {
      if (errno != EACCES && errno != ENOENT && errno != ENXIO) {
        perror("Error opening public fifo");
        exit(EXIT_FAILURE);
      }
    }
  }

  atexit(&closePubFifo);
  time_t seed = time(NULL);

  while (getRemaining() > 0 && serverOpen) {  // Time remaining
    // Pseudo random interval between thread creation
    int rand_numb =
        ((rand_r((unsigned int *)(&seed)) % 1000) * 1000);
        // random milissecond number, from 0 ms to 1
    if (usleep(rand_numb) == -1) {
      perror("Error usleep");
      exit(EXIT_FAILURE);
    }
    if (pthread_create(&tid, &detatched, (void *)(&cThreadFunc),
                       (void *)(&taskId)) != 0) {
      perror("Error creating threads");
      exit(EXIT_FAILURE);
    }

    taskId++;
  }

  // Destroy detached threads setup
  pthread_attr_destroy(&detatched);

  pthread_exit(0);
}
