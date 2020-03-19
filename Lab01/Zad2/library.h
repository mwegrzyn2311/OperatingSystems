#ifndef DIFF_LIBRARY_H
#define DIFF_LIBRARY_H

#include <stdbool.h>
#include <ctype.h>
#include <string.h>

struct Block {
    char** operations;
    int size;
};

struct BlockArray {
    struct Block **blocks;
    int size;
    int freeIndex;
    int lastIndex;
};

struct FilePair {
    char* file1;
    char* file2;
};
struct FileSequence {
    struct FilePair* pairs;
    int size;
};

#ifndef DYNAMIC
struct BlockArray* createBlockArray(int size);
int createBlockAndAddToArray(struct BlockArray* array, struct FilePair files);
int amountOfOperationInBlock(struct Block block);
void removeBlock(struct BlockArray* array, int index);
void removeOperation(struct BlockArray* array, int blockIndex, int operationIndex);
bool startsWithNumber(char* text);
struct FileSequence* createFileSequence(int size, char** input);
void comparePairs(struct BlockArray* array, int size, char** input);
void printAllBlocks(struct BlockArray array);
void printAllOperations(struct BlockArray array, int blockIndex);
void freeBlockArray(struct BlockArray* array);
int operationAmount(struct BlockArray *array);
#endif

#endif