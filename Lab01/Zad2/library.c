#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool startsWithNumber(char* text) {
    return (strlen(text) > 0 && isdigit(text[0]));
}

struct BlockArray* createBlockArray(int size) {
    if(size <= 0) {
        return NULL;
    }
    struct BlockArray* array = (struct BlockArray*) calloc(1,sizeof(struct BlockArray));
    array->blocks = (struct Block**) calloc(size, sizeof(struct Block*));
    array->size = size;
    array->freeIndex = 0;
    array->lastIndex = -1;
    return array;
}

int createBlockAndAddToArray(struct BlockArray* array, struct FilePair files) {
    if(array->freeIndex == array->size) {
        printf("Cannot add another block to the array as it is full\n");
        return -1;
    }

    struct Block* newBlock = (struct Block*) calloc(1, sizeof(struct Block));
    int MAX_LEN = 200000;
    int MAX_COMMAND_LEN = 20000;
    int MAX_OPERATION_AMOUNT = 40000;
    char* tmp = (char*) calloc(MAX_LEN, sizeof(char));
    newBlock->operations = (char**) calloc(MAX_OPERATION_AMOUNT, sizeof(char*));
    char* command = (char*) calloc(MAX_COMMAND_LEN, sizeof(char));
    strcpy(command, "diff ");
    strcat(command, files.file1);
    strcat(command, " ");
    strcat(command, files.file2);
    FILE *file;
    printf("Executing: %s\n", command);
    file = popen(command, "r");
    if(file == NULL) {
        printf("couldn't have executed the command\n");
        exit(0);
    }
    int index = -1;
    while(fgets(tmp, MAX_LEN, file) != NULL) {
        if(startsWithNumber(tmp)) {
            newBlock->operations[++index] = calloc(MAX_LEN, sizeof(char));
        }
        strcat(newBlock->operations[index], tmp);
    }
    newBlock->size = index+1;
    int status = pclose(file);
    if (status == -1) {
        printf("Temporary file failed closing with status %d\n", status);
        exit(0);
    }
    array->blocks[array->freeIndex] = newBlock;
    int res = array->freeIndex;
    if(array->freeIndex > array->lastIndex && array->freeIndex < array->size)
        array->lastIndex = array->freeIndex;
    while(array->freeIndex <= array->lastIndex) {
        //TODO: Fix
        if(array->blocks[array->freeIndex] == NULL)
            break;
        array->freeIndex++;
    }
    return res;
}

int amountOfOperationInBlock(struct Block block) {
    int res = 0;
    for(int i = 0; i < block.size; i++) {
        if(block.operations[i] != NULL)
            res++;
    }
    return res;
}
int operationAmount(struct BlockArray *array) {
    int res = 0;
    for(int i = 0; i < array->size; i++) {
        res += array->blocks[i]->size;
    }
    return res;
}


void removeBlock(struct BlockArray* array, int index) {
    if(index < 0 || index >= array->size) {
        printf("Cannot remove block at index %d from array of size %d", index, array->size);
        return;
    }
    free(array->blocks[index]->operations);
    array->blocks[index] = NULL;
    if(index < array->freeIndex)
        array->freeIndex = index;
    if(index == array->lastIndex)
        array->lastIndex--;
}

void removeOperation(struct BlockArray* array, int blockIndex, int operationIndex) {
    if(blockIndex < 0 || blockIndex >= array->size) {
        printf("Cannot remove operation from block at index %d inside array of size %d", blockIndex, array->size);
        return;
    }
    if(operationIndex < 0 || operationIndex >= array->blocks[blockIndex]->size) {
        printf("Cannot remove operation at index %d from block of size %d", operationIndex, array->blocks[blockIndex]->size);
        return;
    }
    free(array->blocks[blockIndex]->operations[operationIndex]);
    array->blocks[blockIndex]->operations[operationIndex] = NULL;
}

struct FileSequence* createFileSequence(int size, char** input) {
    struct FileSequence* fileSeq = (struct FileSequence*) calloc(1, sizeof(struct FileSequence));
    fileSeq->size = size;
    fileSeq->pairs = (struct FilePair*) calloc(size, sizeof(struct FilePair));
    for(int i = 0; i < size; i++) {
        char *dividingSign = strchr(input[i], ':');
        if(dividingSign == NULL) {
            printf("Wrong input format. Use FileName1:FileName2 for each file pair instead.\n");
            return NULL;
        }
        int len1 = dividingSign - input[i] +1;
        int len2 = strlen(dividingSign+1) +1;

        fileSeq->pairs[i].file1 = (char*) calloc(len1, sizeof(char));
        fileSeq->pairs[i].file2 = (char*) calloc(len2, sizeof(char));

        strncpy(fileSeq->pairs[i].file1, input[i], len1);
        fileSeq->pairs[i].file1[len1-1] = '\0';
        strncpy(fileSeq->pairs[i].file2, dividingSign+1, len2);
    }
    return fileSeq;
}

void comparePairs(struct BlockArray* array, int size, char** input) {
    if(size <= 0) {
        return;
    }
    struct FileSequence* fileSeq = createFileSequence(size, input);
    for(int i = 0; i < fileSeq->size; i++) {
        createBlockAndAddToArray(array, fileSeq->pairs[i]);
    }
}

void printAllBlocks(struct BlockArray array) {
    printf("Printing all operations from all blocks from given array\n");
    for(int i = 0; i < array.size; i++) {
        printf("Printing all operations from block at index %d\n", i);
        for(int j = 0; j < array.blocks[i]->size; j++) {
            printf(array.blocks[i]->operations[j]);
        }
        printf("\n");
    }
}

void printAllOperations(struct BlockArray array, int blockIndex) {
    printf("Printing all operations from block at index %d\n", blockIndex);
    for(int i = 0; i < array.blocks[blockIndex]->size; i++) {
        printf(array.blocks[blockIndex]->operations[i]);
    }
    printf("\n");
}

void freeBlockArray(struct BlockArray* array) {
    if(array == NULL)
        return;
    for(int i = 0; i < array->size; i++) {
        if (array->blocks[i] != NULL) {
            for (int j = 0; j < array->blocks[i]->size; j++) {
                free(array->blocks[i]->operations[j]);
            }
        }
        free(array->blocks[i]);
    }
    free(array);
}
