#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void generate(char* dane, int records, long size) {
    srand(time(NULL));

    FILE* file = fopen(dane, "w");
    char* record = (char*) calloc(size, sizeof(char));
    for(int i = 0; i < records; ++i) {
        for(long j = 0; j < size; ++j) {
            // record[j] = (char) (rand()%93 + 33);
            record[j] = (char) (rand()%25 + 65);
        }
        fprintf(file, "%s\n", record);
    }
    fclose(file);
    free(record);
}


void swap_lines_sys(int file, int pos1, int pos2, long size) {
    char* record1 = (char*) calloc(size, sizeof(char));
    char* record2 = (char*) calloc(size, sizeof(char));
    lseek(file, pos1, 0);
    read(file, record1, size);
    lseek(file, pos2, 0);
    read(file, record2, size);
    lseek(file, pos1, 0);
    write(file, record2, size);
    lseek(file, pos2, 0);
    write(file, record1, size);
    free(record1);
    free(record2);
}

int part_sys(int file, int left, int right, long size) {
    //pivot is the right element
    char* pivot = (char*) calloc(size, sizeof(char));
    char* record = (char*) calloc(size, sizeof(char));
    lseek(file, right*size, 0);
    read(file, pivot, size);
    int res = left;
    lseek(file, left*size, 0);
    for(int i = left; i < right; i++) {
        read(file, record, size);
        if(strcmp(pivot, record) > 0) {
            swap_lines_sys(file, res*size, i*size, size);
            res++;
        }
    }
    swap_lines_sys(file, res*size, right*size, size);
    free(record);
    free(pivot);
    return res;
}

void qs_sys(int file, int left, int right, long size) {
    if(left < right) {
        int pi = part_sys(file, left, right, size);
        qs_sys(file, left, pi-1, size);
        qs_sys(file, pi+1, right, size);
    }
}

void sort_sys(char* fileName, int records, long size) {
    int file = open(fileName, O_RDWR);
    size++; //We add \n after each record, so we need to do size++
    qs_sys(file, 0, records-1, size);
    close(file);
}


void swap_lines_lib(FILE* file, int pos1, int pos2, long size) {
    char* record1 = (char*) calloc(size, sizeof(char));
    char* record2 = (char*) calloc(size, sizeof(char));
    fseek(file, pos1, 0);
    fread(record1, sizeof(char), size, file);
    fseek(file, pos2, 0);
    fread(record2, sizeof(char), size, file);
    fseek(file, pos1, 0);
    fwrite(record2, sizeof(char), size, file);
    fseek(file, pos2, 0);
    fwrite(record1, sizeof(char), size, file);
    free(record1);
    free(record2);
}

int part_lib(FILE* file, int left, int right, long size) {
    //pivot is the right element
    char* pivot = (char*) calloc(size, sizeof(char));
    char* record = (char*) calloc(size, sizeof(char));
    fseek(file, right*size, 0);
    fread(pivot, sizeof(char), size, file);
    int res = left;
    fseek(file, left*size, 0);
    for(int i = left; i < right; i++) {
        fread(record, sizeof(char), size ,file);
        if(strcmp(pivot, record) > 0) {
            swap_lines_lib(file, res*size, i*size, size);
            res++;
        }
    }
    swap_lines_lib(file, res*size, right*size, size);
    return res;
}

void qs_lib(FILE* file, int left, int right, long size) {
    if(left < right) {
        int pi = part_lib(file, left, right, size);
        qs_lib(file, left, pi-1, size);
        qs_lib(file, pi+1, right, size);
    }
}

void sort_lib(char* fileName, int records, long size) {
    size++; //We add \n after each record, so we need to do size++
    FILE* file = fopen(fileName, "r+");
    char* pivot = (char*) calloc(size, sizeof(char));
    char* record = (char*) calloc(size, sizeof(char));
    fseek(file, (records-1)*size, 0);
    fread(pivot, sizeof(char), size, file);
    qs_lib(file, 0, records-1, size);
    fclose(file);
}


void copy_sys(char* fileName1, char* fileName2, int records, long size) {
    int file1 = open(fileName1, O_RDONLY);
    int file2 = open(fileName2, O_WRONLY);
    size++; //We add \n after each record, so we need to do size++
    char* record = (char*) calloc(size, sizeof(char));
    for(int i = 0; i < records; ++i) {
        read(file1, record, size);
        write(file2, record, size);
    }
    close(file1);
    close(file2);
    free(record);
}
void copy_lib(char* fileName1, char* fileName2, int records, long size) {
    FILE* file1 = fopen(fileName1, "r");
    FILE* file2 = fopen(fileName2, "w");
    size++; //We add \n after each record, so we need to do size++
    if(!file1 || !file2) {

    }
    char* record = (char*) calloc(size, sizeof(char));
    for(int i = 0; i < records; ++i) {
        int len = fread(record, sizeof(char), size, file1);
        fwrite(record, sizeof(char), len, file2);
    }
    fclose(file1);
    fclose(file2);
    free(record);
}