#include "timer.h"

#include <time.h>

long int start;
long int end;

long int getTime(){
    return time(NULL);
}

void setTimer(int duration){
    start = time(NULL);
    end = start + duration;
}

long int getElapsed(){
    return getTime() - start;
}

long int getRemaining(){
    return end - getTime();
}


// podiamos usar posix timers
