#include <stdio.h>

#include "cmd_parser.h"

int main(int argc, char* const argv[]){
    int nsecs;
    char* fifoname;

    if (cmdParser(argc, argv, &nsecs, &fifoname) != 0){
        return 1;
    }

    printf("nsecs = %d, fifoname = %s\n", nsecs, fifoname);
    return 0;
}
