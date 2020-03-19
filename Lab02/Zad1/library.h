#ifndef LAB02_LIBRARY_H
#define LAB02_LIBRARY_H

void generate(char*fileName, int records, long size);

void sort_sys(char* fileName, int records, long size);
void sort_lib(char* fileName, int records, long size );

void copy_sys(char* fileName1, char* fileName2, int records, long size);
void copy_lib(char* fileName1, char* fileName2, int records, long size);

#endif