#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <zconf.h>
#include "library.h"
#ifdef DYNAMIC
#include <dlfcn.h>

    struct BlockArray* (*createBlockArray)(int);
    void (*removeOperation)(struct BlockArray*, int, int);
    void (*removeBlock)(struct BlockArray*, int);
    void (*comparePairs)(struct BlockArray*, int, char**);
    void (*printAllBlocks)(struct BlockArray);
    void (*printAllOperations)(struct BlockArray, int);
    void (*freeBlockArray)(struct BlockArray*);
    int (*operationAmount)(struct BlockArray*);

#endif
FILE* raport;

double timeDelta(clock_t start, clock_t end){
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void getPrintAndOverrideTimeDelta(clock_t start, clock_t end, struct tms* tmsStart, struct tms* tmsEnd){
    printf("\tREAL-TIME: %.3lf\n", timeDelta(start,end));
    printf("\tUSER-TIME: %.3lf\n", timeDelta(tmsStart->tms_utime, tmsEnd->tms_utime));
    printf("\tSYSTEM-TIME: %.3lf\n", timeDelta(tmsStart->tms_stime, tmsEnd->tms_stime));

    fprintf(raport, "\tREAL-TIME: %.3lf\n", timeDelta(start,end));
    fprintf(raport, "\tUSER-TIME: %.3lf\n", timeDelta(tmsStart->tms_utime, tmsEnd->tms_utime));
    fprintf(raport, "\tSYSTEM-TIME: %.3lf\n", timeDelta(tmsStart->tms_stime, tmsEnd->tms_stime));
}

void measureTimeForGivenDataSet(char** dataSet, int size) {
    struct BlockArray* array = createBlockArray(size);

    int amountOfBlocksToRemove = size-2;
    int timeStepsAmount = 5;
    struct tms** tmsTime = (struct tms**) calloc(timeStepsAmount, sizeof(struct tms *));
    clock_t* realTime = calloc(timeStepsAmount, sizeof(clock_t));
    for(int i = 0; i < timeStepsAmount; i++) {
        tmsTime[i] = (struct tms*) calloc(1, sizeof(struct tms));
    }
    realTime[0] = times(tmsTime[0]);
    comparePairs(array, size, dataSet);
    realTime[1] = times(tmsTime[1]);
    for(int i = 0; i < amountOfBlocksToRemove; i++){
        removeBlock(array, i);
    }
    realTime[2] = times(tmsTime[2]);
    comparePairs(array, size, dataSet);
    realTime[3] = times(tmsTime[3]);
    printAllBlocks(*array);
    realTime[4] = times(tmsTime[4]);

    printf("Comparing %d pairs time, consisting %d operations\n", size, operationAmount(array));
    fprintf(raport, "Time measured for comparing %d given file pairs, consisting %d operations:\n", size, operationAmount(array));
    getPrintAndOverrideTimeDelta(realTime[0], realTime[1], tmsTime[0], tmsTime[1]);

    printf("Deleting %d blocks time\n", amountOfBlocksToRemove);
    fprintf(raport, "Time measured for deleting %d blocks:\n", amountOfBlocksToRemove);
    getPrintAndOverrideTimeDelta(realTime[1], realTime[2], tmsTime[1], tmsTime[2]);


    printf("Comparing %d pairs for the second time time (after removing %d blocks from the main array)\n", size, amountOfBlocksToRemove);
    fprintf(raport, "Time measured for comparing %d file pairs for the second time (after removing %d blocks from the main array):\n", size, amountOfBlocksToRemove);
    getPrintAndOverrideTimeDelta(realTime[2], realTime[3], tmsTime[2], tmsTime[3]);

    printf("Printing all blocks time\n");
    fprintf(raport, "Time measured for printing all the blocks to console:\n");
    getPrintAndOverrideTimeDelta(realTime[3], realTime[4], tmsTime[3], tmsTime[4]);

    fprintf(raport, "\n");
    freeBlockArray(array);
}

char* smallSimilar[] = {"a.txt:b.txt", "a.txt:c.txt", "b.txt:c.txt"};
char* smallPartlySimilar[] = {"a.txt:k.txt", "f.txt:g.txt", "lorem6.txt:lorem7.txt", "a.txt:lorem1.txt"};
char* smallDifferent[] = {"a.txt:lorem1.txt", "d.txt:lorem2.txt", "f.txt:lorem8.txt", "k.txt:lorem3.txt"};
char* mediumSimilar[] = {"a.txt:b.txt", "a.txt:c.txt", "b.txt:c.txt", "a.txt:d.txt", "a.txt:e.txt", "b.txt:f.txt", "e.txt:h.txt", "e.txt:j.txt", "e.txt:k.txt", "f.txt:h.txt", "f.txt:j.txt", "f.txt:k.txt",
                         "g.txt:h.txt", "g.txt:j.txt", "g.txt:k.txt", "h.txt:h.txt", "h.txt:j.txt", "h.txt:k.txt", "i.txt:h.txt", "i.txt:j.txt", "i.txt:k.txt",  "k.txt:h.txt", "k.txt:j.txt", "k.txt:k.txt"};
char* mediumPartlySimilar[] = {"a.txt:b.txt", "a.txt:c.txt", "b.txt:c.txt", "a.txt:d.txt", "a.txt:e.txt", "b.txt:f.txt", "e.txt:h.txt", "e.txt:j.txt", "e.txt:k.txt", "f.txt:h.txt", "f.txt:j.txt", "f.txt:k.txt",
                               "g.txt:lorem1.txt", "g.txt:lorem2.txt", "g.txt:lorem3.txt", "h.txt:lorem4.txt", "h.txt:lorem5.txt", "h.txt:lorem6.txt", "i.txt:lorem7.txt", "i.txt:lorem8.txt", "i.txt:lorem9.txt", "k.txt:lorem10.txt", "k.txt:j.txt", "k.txt:k.txt"};
char* mediumDifferent[] = {"a.txt:lorem1.txt", "a.txt:lorem2.txt", "b.txt:lorem3.txt", "a.txt:lorem4.txt", "a.txt:lorem5.txt", "b.txt:lorem6.txt", "e.txt:lorem7.txt", "e.txt:lorem8.txt", "e.txt:lorem9.txt", "f.txt:lorem10.txt", "f.txt:lorem1.txt", "f.txt:k.txt",
                           "g.txt:lorem1.txt", "g.txt:lorem2.txt", "g.txt:lorem3.txt", "h.txt:lorem4.txt", "h.txt:lorem5.txt", "h.txt:lorem6.txt", "i.txt:lorem7.txt", "i.txt:lorem8.txt", "i.txt:lorem9.txt", "k.txt:lorem10.txt", "k.txt:j.txt", "k.txt:k.txt"};
char* bigSimilar[] = {"a.txt:b.txt", "a.txt:c.txt", "b.txt:c.txt", "a.txt:d.txt", "a.txt:e.txt", "b.txt:f.txt", "e.txt:h.txt", "e.txt:j.txt", "e.txt:k.txt", "f.txt:h.txt", "f.txt:j.txt", "f.txt:k.txt",
                      "g.txt:h.txt", "g.txt:j.txt", "g.txt:k.txt", "h.txt:h.txt", "h.txt:j.txt", "h.txt:k.txt", "i.txt:h.txt", "i.txt:j.txt", "i.txt:k.txt",  "k.txt:h.txt", "k.txt:j.txt", "k.txt:k.txt",
                      "j.txt:h.txt", "g.txt:j.txt", "j.txt:k.txt", "h.txt:h.txt", "j.txt:j.txt", "d.txt:k.txt", "d.txt:h.txt", "d.txt:j.txt", "g.txt:a.txt",  "g.txt:b.txt", "g.txt:c.txt", "g.txt:d.txt",
                      "k.txt:a.txt", "k.txt:b.txt", "k.txt:c.txt", "k.txt:d.txt", "k.txt:e.txt", "i.txt:a.txt", "i.txt:b.txt", "i.txt:c.txt", "i.txt:d.txt",  "i.txt:e.txt", "k.txt:f.txt", "i.txt:g.txt"};
char* bigPartlySimilar[] = {"a.txt:b.txt", "a.txt:c.txt", "b.txt:c.txt", "a.txt:d.txt", "a.txt:e.txt", "b.txt:f.txt", "e.txt:h.txt", "e.txt:j.txt", "e.txt:k.txt", "f.txt:h.txt", "f.txt:j.txt", "f.txt:k.txt",
                            "g.txt:h.txt", "g.txt:j.txt", "g.txt:k.txt", "h.txt:h.txt", "h.txt:j.txt", "h.txt:k.txt", "i.txt:h.txt", "i.txt:j.txt", "i.txt:k.txt",  "k.txt:h.txt", "k.txt:j.txt", "k.txt:k.txt",
                            "b.txt:lorem1.txt", "b.txt:lorem2.txt", "c.txt:lorem3.txt", "b.txt:lorem4.txt", "b.txt:lorem5.txt", "d.txt:lorem6.txt", "d.txt:lorem7.txt", "d.txt:lorem8.txt", "f.txt:lorem9.txt", "g.txt:lorem10.txt", "g.txt:lorem1.txt", "h.txt:lorem4.txt",
                            "k.txt:lorem1.txt", "k.txt:lorem2.txt", "k.txt:lorem3.txt", "k.txt:lorem4.txt", "k.txt:lorem5.txt", "j.txt:lorem6.txt", "j.txt:lorem7.txt", "j.txt:lorem8.txt", "j.txt:lorem9.txt", "j.txt:lorem10.txt", "j.txt:lorem4.txt", "j.txt:lorem1.txt"};
char* bigDifferent[] = {"a.txt:lorem1.txt", "a.txt:lorem2.txt", "b.txt:lorem3.txt", "a.txt:lorem4.txt", "a.txt:lorem5.txt", "b.txt:lorem6.txt", "e.txt:lorem7.txt", "e.txt:lorem8.txt", "e.txt:lorem9.txt", "f.txt:lorem10.txt", "f.txt:lorem1.txt", "f.txt:lorem4.txt",
                        "g.txt:lorem1.txt", "g.txt:lorem2.txt", "g.txt:lorem3.txt", "h.txt:lorem4.txt", "h.txt:lorem5.txt", "h.txt:lorem6.txt", "i.txt:lorem7.txt", "i.txt:lorem8.txt", "i.txt:lorem9.txt", "k.txt:lorem10.txt", "k.txt:lorem4.txt", "k.txt:lorem1.txt",
                        "b.txt:lorem1.txt", "b.txt:lorem2.txt", "c.txt:lorem3.txt", "b.txt:lorem4.txt", "b.txt:lorem5.txt", "d.txt:lorem6.txt", "d.txt:lorem7.txt", "d.txt:lorem8.txt", "f.txt:lorem9.txt", "g.txt:lorem10.txt", "g.txt:lorem1.txt", "h.txt:lorem4.txt",
                        "k.txt:lorem1.txt", "k.txt:lorem2.txt", "k.txt:lorem3.txt", "k.txt:lorem4.txt", "k.txt:lorem5.txt", "j.txt:lorem6.txt", "j.txt:lorem7.txt", "j.txt:lorem8.txt", "j.txt:lorem9.txt", "j.txt:lorem10.txt", "j.txt:lorem4.txt", "j.txt:lorem1.txt"};
int sizes[] = {3, 4, 4, 24, 24, 24, 48, 48, 48};

int main(int argc, char** argv) {

#ifdef DYNAMIC

    void* lib = dlopen("./library.so", RTLD_NOW);
    if( !lib ) {
        exit(1);
    }
    createBlockArray = dlsym(lib, "createBlockArray");
    removeBlock = dlsym(lib, "removeBlock");
    removeOperation = dlsym(lib, "removeOperation");
    comparePairs = dlsym(lib, "comparePairs");
    printAllBlocks = dlsym(lib, "printAllBlocks");
    printAllOperations = dlsym(lib, "printAllOperations");
    freeBlockArray = dlsym(lib, "freeBlockArray");
    operationAmount = dlsym(lib, "operationAmount");

#endif

    if(argc == 2) {
        raport = fopen(argv[1], "a");
    } else {
        raport = fopen("raport2.txt", "a");
    }
    int i = 0;

    fprintf(raport, "TIME MEASUREMENT RESULTS for small-similar data set\n");
    measureTimeForGivenDataSet(smallSimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for small-partly-similar data set\n");
    measureTimeForGivenDataSet(smallPartlySimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for small-different data set\n");
    measureTimeForGivenDataSet(smallDifferent, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for medium-similar data set\n");
    measureTimeForGivenDataSet(mediumSimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for medium-partly-similar data set\n");
    measureTimeForGivenDataSet(mediumPartlySimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for medium-different data set\n");
    measureTimeForGivenDataSet(mediumDifferent, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for big-similar data set\n");
    measureTimeForGivenDataSet(bigSimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for big-partly-similar data set\n");
    measureTimeForGivenDataSet(bigPartlySimilar, sizes[i++]);

    fprintf(raport, "TIME MEASUREMENT RESULTS for big-different data set\n");
    measureTimeForGivenDataSet(bigDifferent, sizes[i++]);
}
