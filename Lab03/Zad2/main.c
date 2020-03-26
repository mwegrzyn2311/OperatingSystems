#include <stdio.h>
#include "library.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char** argv) {
    if(argc < 5) {
        printf("Not enough arguments\n");
        exit(1);
    }
    char* fileList = argv[1];
    int workersCount = atoi(argv[2]);
    int seconds = atoi(argv[3]);
    bool writeToSharedFile;
    if(argc >= 5 && strcmp(argv[4], "shared") == 0) {
        writeToSharedFile = true;
    } else {
        writeToSharedFile = false;
    }
    multiplyWholeList(fileList, workersCount, seconds, writeToSharedFile);
    return 0;
}