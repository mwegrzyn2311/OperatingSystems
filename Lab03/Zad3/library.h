#ifndef ZAD2_LIBRARY_H
#define ZAD2_LIBRARY_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char* filename;
    size_t width;
    size_t height;
    int** values;
}FileMatrix;

void multiplyMatrices(char *filename1, char *filename2, char *resFilename, int workersCount, int seconds, bool shared);
void multiplyWholeList(char* fileList, int workersCount, int seconds, bool shared);
#endif