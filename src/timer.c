#include "timer.h"

#include <time.h>

long int start;

long int get_time(){
    return time(NULL);
}

void set_start(){
    start = time(NULL);
}

long int get_elapsed(){
    return get_time() - start;
}