#include <stdio.h>
#include "library.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/resource.h>

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
    if(argc >=7) {
        int cpuLimit = atoi(argv[5]);
        int asLimit = atoi(argv[6]);
        struct rlimit cpu = {cpuLimit, cpuLimit};
        struct rlimit as = {asLimit*1000000, asLimit*1000000};

        setrlimit(RLIMIT_CPU, &cpu);
        setrlimit(RLIMIT_AS, &as);
    }
    
    multiplyWholeList(fileList, workersCount, seconds, writeToSharedFile);
    return 0;
}