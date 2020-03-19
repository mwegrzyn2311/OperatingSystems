#include <stdio.h>
#include <stdlib.h>
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

bool isNumber(char* s) {
    for (int i = 0; i < strlen(s); i++)
        if (isdigit(s[i]) == false)
            return false;

    return true;
}
bool isSize(char* arg) {
    return (isNumber(arg) && strtol(arg, NULL, 10) > 0);
}

char* correctCommands[5]={"compare_pairs", "remove_block", "remove_operation", "print_all_blocks", "print_operations_from_block"};
// Returns command's index if it was correctCommand, otherwise returns -1
int getCommandIndex(char* string) {
    for(int i = 0; i < 5; i++) {
        if(strcmp(string, correctCommands[i]) == 0)
            return i;
    }
    return -1;
}

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

    if(argc < 3 || strcmp(argv[1], "create_table") != 0 || !isSize(argv[2])) {
        printf("Too few arguments or wrong format");
        exit(1);
    }
    int blockArraySize = strtol(argv[2], NULL, 10);
    struct BlockArray* array = createBlockArray(blockArraySize);

    //TODO: Check if lastCommand doesn't have to be initialized as longer string
    int i = 3;
    while(i < argc) {
        int commandIndex = getCommandIndex(argv[i]);
        if(commandIndex == -1) {
            printf("Wrong format\n");
            exit(1);
        }

        printf("%s\n", argv[i]);

        if(commandIndex == 0) {
            int size = 0;
            int j = ++i;
            while(i < argc && getCommandIndex(argv[i]) == -1) {
                size++;
                i++;
            }
            if(size == 0) {
                printf("No files specified for file comparison\n");
            }
            char** input = (char**) calloc(size, sizeof(char*));
            for(int k = 0; k < size; k++) {
                input[k] = (char*) calloc(strlen(argv[j+k]), sizeof(char));
                strcpy(input[k], argv[j+k]);
            }
            comparePairs(array, size, input);
        } else if (commandIndex == 1) {
            i++;
            if(!isNumber(argv[i])) {
                printf("Cannot remove block from non-numeric index");
                exit(1);
            }
            int blockIndex = strtol(argv[i], NULL, 10);
            removeBlock(array, blockIndex);
            i++;
        } else if(commandIndex == 2) {
            i++;
            if(!isNumber(argv[i])) {
                printf("Cannot remove block from non-numeric index");
                exit(1);
            }
            int blockIndex = strtol(argv[i], NULL, 10);
            i++;
            if(!isNumber(argv[i])) {
                printf("Cannot remove operation from non-numeric index");
                exit(1);
            }
            int operationIndex = strtol(argv[i], NULL, 10);
            removeOperation(array, blockIndex, operationIndex);
            i++;
        } else if(commandIndex == 3) {
            printAllBlocks(*array);
            i++;
        } else if(commandIndex == 4) {
            i++;
            if(!isNumber(argv[i])) {
                printf("Cannot print block from non-numeric index");
                exit(1);
            }
            int blockIndex = strtol(argv[i], NULL, 10);
            printAllOperations(*array, blockIndex);
            i++;
        }
    }

    freeBlockArray(array);
    return 0;
}
