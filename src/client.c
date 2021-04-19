#include <stdio.h>
struct timespec start_time;
struct timespec end_time;

void create_thread(int nsecs, char *fifoname, int *seq_i) {
  int rand_numb = (rand() % 10000 ) + 10000; // números entre 0 e 10000 + 10000 
  
  pthread_t tid;
    
  pthread_create(&tid, NULL, client, (void*) seq_i); //Há também a chamada de uma função client, que usará vários dos parâmetros passados na função/via apontadores?
  pthread_join(tid, NULL);

  //supostamente para termos threads que são criadas em intervalos de tempo pseudo-random
  usleep(rand_numb);
}



// retornar tempo que já passou, desde o início do progrma, comparando assim com nsecs dado no início
//usar clock_gettime com como parâmetro end_time antes de chamar esta função, para obter tempo actual e assim comparar com o tempo de ínicio
int (struct timespec start_time, struct timespec end_time) {

   accum_time = ((end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec ) )/ 1000000000.0;

   return accum_time;
}





int main(int argc, char* const argv[]){
    int nsecs;
    char* fifoname;

    if (cmdParser(argc, argv, &nsecs, &fifoname) != 0){
        return 1;
    }

    printf("nsecs = %d, fifoname = %s\n", nsecs, fifoname);
    return 0;
}
