#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "library.h"

int main(int argc, char** argv) {
    if(argc < 5) {
        printf("Podano zbyt mało argumentów.\n");
        return -1;
    }
    if(strcmp(argv[1],"generate") == 0) {
        char* filename = argv[2];
        char* file = (char*) calloc(strlen(filename)+4, sizeof(char));
        strcpy(file,filename);
        strcat(file, ".txt");
        int count = atoi(argv[3]);
        int size = atoi(argv[4]);
        generate(file, count, size);
    } else if(strcmp(argv[1],"copy") == 0) {
        if(argc == 5) {
            printf("Too few arguments to call copy\n");
            return -1;
        }
        char* filename = argv[2];
        char* file1 = (char*) calloc(strlen(filename)+4, sizeof(char));
        strcpy(file1,filename);
        strcat(file1, ".txt");
        char* filename2 = argv[3];
        char* file2 = (char*) calloc(strlen(filename2)+4, sizeof(char));
        strcpy(file2,filename2);
        strcat(file2, ".txt");
        int count = atoi(argv[4]);
        int size = atoi(argv[5]);
        if(argc >= 6 && strcmp(argv[6],"sys") == 0) {
            copy_sys(file1, file2, count, size);
        } else {
            copy_lib(file1, file2, count, size);
        }
    } else if(strcmp(argv[1],"sort") == 0) {
        char* filename = argv[2];
        char* file = (char*) calloc(strlen(filename)+4, sizeof(char));
        strcpy(file,filename);
        strcat(file, ".txt");
        int count = atoi(argv[3]);
        int size = atoi(argv[4]);
        if(argc >= 5 && strcmp(argv[5],"sys") == 0) {
            sort_sys(file, count, size);
        } else {
            sort_lib(file, count, size);
        }
    } else {
        printf("Wrong operation. Correct are: generate, copy and sort.\n");
        return -1;
    }
    return 0;
}