#include "fifo.h"
#include "common.h" //message

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>

int fifo(int id, int load, char* fifoname) {
	time_t t;
		
	Message message;
	message.rid = id;
	message.tskload = load;
	message.pid = getpid();
	message.tid = pthread_self();
	message.tskres = -1;

	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	
	//esperar que o fifo seja criado antes de tentar abrir
	int fd = open(fifoname, O_WRONLY);
	if (fd == -1) {
		return 1;
	}

	//podem surgir race conditions visto que o fifo é público e pode ser acedido por todas as threads
	pthread_mutex_lock(&lock);
	if (write(fd, &message, sizeof(Message)) == -1) {
		printf("write error %d\n",errno);
		return 2;
	}
	pthread_mutex_unlock(&lock);

	time(&t);
	printf("%s ; %d ; %d ; %d;  %ld; %d; IWANT\n", ctime(&t), message.rid, message.tskload, message.pid, message.tid, message.tskres);
	close(fd);

	char privatefifoname[1000] = "/tmp/";
	char pidstr[1000];
	char tidstr[1000];
	sprintf(pidstr,"%d",message.pid);
	sprintf(tidstr,"%ld",message.tid);
	strcat(privatefifoname, pidstr);
	strcat(privatefifoname, ".");
	strcat(privatefifoname, tidstr);

	if (mkfifo(privatefifoname, 0777) != 0) {//-1
		if (errno != EEXIST) {
			printf("could not create fifo file\n");
			return 1;
		}
	}

	int fd2 = open(privatefifoname, O_RDONLY);
	if (fd2 == -1) {
		printf("open error %d\n",errno);
		return 1;
	}

	int result;
	if (read(fd2, &message, sizeof(Message)) == -1) {
		printf("read error %d\n",errno);
		return 2;
	}

	close(fd2);

	time(&t);
	if (message.tskres != -1) {
		printf("%s ; %d ; %d ; %d ; %ld ; %d ; GOTRS\n", ctime(&t), message.rid, message.tskload, message.pid, message.tid, message.tskres);
	}
	else {
		printf("%s ; %d ; %d ; %d ; %ld ; %d ; CLOSD\n", ctime(&t), message.rid, message.tskload, message.pid, message.tid, message.tskres);
		return 3;
	}

	if(remove(privatefifoname)!=0){
		printf("Private FIFO wasn't removed!\n");
	}

	return 0;
}