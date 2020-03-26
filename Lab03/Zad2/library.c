#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

size_t MAX_FILENAME_LEN = 200;
size_t NUMBER_LEN = 20;
char* tasksPath = "tasks.txt";


size_t amountOfLines(FILE* f) {
    size_t res = 0;
    fseek(f, 0, 0);
    size_t maxLen = MAX_FILENAME_LEN*3+2;
    char* buffer = malloc(maxLen*sizeof(char));
    while(fgets(buffer, maxLen, f) != NULL) {
        res++;
    }
    free(buffer);
    fseek(f, 0, 0);
    return res;
}

size_t longLongLen(long long int a) {
    if(a == 0) {
        return 1;
    }
    size_t res = 0;
    if(a < 0) {
        //sign
        res++;
        a = -a;
    }
    while(a > 0) {
        res++;
        a /= 10;
    }
    return res;
}

void freeFileMatrix(FileMatrix* matrix) {
    for(size_t i = 0; i < matrix->height; i++) {
        free(matrix->values[i]);
    }
    free(matrix->values);
    free(matrix);
}

/** Utility functions to measure matrix dimensions */
size_t getMatrixWidth(FILE* file) {
    fseek(file, 0, 0);
    size_t res = 1;
    char ch;
    do {
        ch = (char) fgetc(file);
        if(ch == ' ') {
            res++;
        }
    } while(ch != EOF && ch != '\n');
    fseek(file, 0, 0);
    return res;
}
size_t getMatrixHeight(FILE * file) {
    fseek(file, 0, 0);
    size_t res = 1;
    char ch;
    do {
        ch = (char) fgetc(file);
        if(ch == '\n') {
            res++;
        }
    } while(ch != EOF);
    fseek(file, 0, 0);
    return res;
}
/** Functions to help set up new matrix */
void setMatrixValues(FileMatrix* matrix, FILE* file) {
    matrix->values = malloc(matrix->height * sizeof(int*));
    fseek(file, 0, 0);
    size_t i,j;
    int a;
    for(i = 0; i < matrix->height; i++) {
        matrix->values[i] = malloc(matrix->width * sizeof(int));
        for(j = 0; j < matrix->width; j++) {
            fscanf(file, "%d", &a);
            matrix->values[i][j] = a;
        }
    }
}

FileMatrix* createMatrixFromFile(char* fileName) {
    FILE* file = fopen(fileName, "r");
    FileMatrix* matrix = malloc(sizeof(FileMatrix));
    matrix->width = getMatrixWidth(file);
    matrix->height = getMatrixHeight(file);
    matrix->filename = fileName;
    setMatrixValues(matrix, file);
    fclose(file);

    return matrix;
}

FileMatrix* createResultMatrix(char* filename, size_t height, size_t width) {
    FileMatrix* matrix = malloc(sizeof(FileMatrix));
    matrix->filename = filename;
    matrix->height = height;
    matrix->width = width;
    matrix->values = malloc(height*sizeof(int*));
    for(size_t i = 0; i < height; i++) {
        matrix->values[i] = malloc(width*sizeof(int));
    }
    return matrix;
}

/** Utilities to print matrix and check what result of multiplication should be */
void printFileMatrix(FileMatrix* matrix) {
    size_t i,j;
    printf("Matrix from file %s:\n", matrix->filename);
    for(i = 0; i < matrix->height; i++) {
        for(j = 0; j < matrix->width-1; j++) {
            printf("%d ", matrix->values[i][j]);
        }
        printf("%d\n", matrix->values[i][j]);
    }
}

void multiplyFileMatrices(FileMatrix *m1, FileMatrix *m2, FileMatrix *res) {
    int sum;
    for(size_t i = 0; i < res->height; i++) {
        for(size_t j = 0; j < res->width; j++) {
            sum = 0;
            for(size_t k = 0; k < m1->width; k++) {
                sum += m1->values[i][k]*m2->values[k][j];
            }
            res->values[i][j] = sum;
        }
    }
}

FileMatrix* getMultipliedMatrix(FileMatrix* m1, FileMatrix* m2, char* resName) {
    if(m1->width != m2->height) {
        printf("Matrices' dimensions don't match\n");
        exit(-4);
    }
    FileMatrix* res = createResultMatrix(resName, m1->height, m2->height);
    multiplyFileMatrices(m1, m2, res);
    return res;
}


void prepareResultFile(char* resPath, size_t height, size_t width) {
    int fd = open(resPath, O_WRONLY | O_CREAT | O_TRUNC);
    for(size_t i = 0; i < height; i++) {
        for(size_t j = 0; j < width; j++) {
            char* buffer;
            size_t len = NUMBER_LEN +1;
            if(i == height-1 && j == width-1) {
                len--;
                buffer = "-9999999999999999999";
            } else {
                if (j == width - 1) {
                    buffer = "-9999999999999999999\n";
                } else {
                    buffer = "-9999999999999999999 ";
                }
            }
            write(fd, buffer, len);
        }
    }
    close(fd);
}



void createTaskFile(size_t width) {
    /** f - > free
     *  b - > busy or finished
     */
    FILE* f = fopen(tasksPath, "w");
    for(size_t i = 0; i < width-1; i++) {
        fwrite("f ", 2, sizeof(char), f);
    }
    fwrite("f ", 1, sizeof(char), f);
    fclose(f);
}

void createColFile(size_t col, size_t height, long long int* colValues) {
    int numLen;
    if (col == 0) {
        numLen = 1;
    } else {
        numLen = (int) log10((double) col)+1;
    }
    char* filename = malloc((18+numLen)*sizeof(char));
    sprintf(filename, "%s%zu%s", "./columns/col", col, ".txt");
    FILE* f = fopen(filename, "w");

    char* buffer = malloc((NUMBER_LEN+3)*sizeof(char));
    for(size_t i = 0; i < height; i++) {
        sprintf(buffer, "%lld", colValues[i]);

        size_t spaces = NUMBER_LEN - longLongLen(colValues[i]);

        for(size_t j = 0; j < spaces; j++) {
            strcat(buffer, " ");
        }

        strcat(buffer, " \n");
        fwrite(buffer, strlen(buffer), sizeof(char), f);
    }
    free(filename);
    free(buffer);
    fclose(f);
}


/** SHARED method section */
void updateCol(char* resFilename, size_t col, size_t height, size_t width, long long int* colValues) {
    int fd = open(resFilename, O_WRONLY);
    flock(fd, LOCK_EX);
    size_t lineLen = width*(NUMBER_LEN+1);
    size_t pos;
    char* buffer = malloc((NUMBER_LEN+2)*sizeof(char));
    for(size_t i = 0; i < height; i++) {
        pos = lineLen*i + (NUMBER_LEN+1)*col;
        lseek(fd, pos, 0);
        sprintf(buffer, "%lld", colValues[i]);
        size_t spaces = NUMBER_LEN - longLongLen(colValues[i]);
    
        for(size_t j = 0; j < spaces; j++) {
            strcat(buffer, " ");
        }
        if(col == width-1) {
            strcat(buffer, "\n");
        } else {
            strcat(buffer, " ");
        }
        write(fd, buffer, NUMBER_LEN+2);
    }
    free(buffer);
    flock(fd, LOCK_UN);
    close(fd);
}


void multiplyCol(FileMatrix* m1, FileMatrix* m2, char* resFilename, size_t col, bool shared) {
    long long int* colValues = malloc(m1->height*sizeof(long long int));
    for(size_t i = 0; i < m1->height; i++) {
        int sum = 0;
        for(size_t j = 0; j < m1->width; j++) {
            sum += m1->values[i][j]*m2->values[j][col];
        }
        colValues[i] = sum;
    }
    if(shared) {
        updateCol(resFilename, col, m1->height, m2->width, colValues);
    } else {
        createColFile(col, m1->height, colValues);
    }
    free(colValues);
}

int getFirstFreeCol(size_t startCol, size_t dimension) {
    /** If column is not finished or taken by another process, takes the column and marks it busy */
    int fd = open(tasksPath, O_RDWR);
    flock(fd, LOCK_EX);
    int i = 0;
    char* buffer = (char*) calloc(1, sizeof(char));
    while(true) {
        size_t curr = startCol+i;
        lseek(fd, curr*2, 0);
        read(fd, buffer, 1);
        if(buffer[0] == 'f') {
            lseek(fd, curr*2, 0);
            write(fd, "b", 1);
            flock(fd, LOCK_UN);
            close(fd);
            free(buffer);
            return (int)curr;
        }
        i++;
        if(curr == dimension-1) {
            i = -(int)startCol;
        }
        if(i==0) {
            flock(fd, LOCK_UN);
            close(fd);
            free(buffer);
            return -1;
        }
    }
}

int multiplyCols(FileMatrix* m1, FileMatrix* m2, char* resFilename, size_t firstCol, size_t cols, int seconds, bool shared) {

    int res = 0;
    clock_t startTime = time(NULL);
    clock_t currentTime;
    size_t currCol = firstCol;
    do {
        int freeCol = getFirstFreeCol(currCol, m2->width);
        if(freeCol == -1) {
            return res;
        }
        multiplyCol(m1, m2, resFilename, (size_t)freeCol, shared);
        res++;
        currentTime = time(NULL);
        if(currentTime - startTime > seconds) {
            return res;
        }
        currCol = (freeCol+1) % (m2->width);
    } while(true);
}

void multiplyShared(FileMatrix* m1, FileMatrix* m2, char* resPath, int workers, int seconds) {
    if(m1->width != m2->height) {
        printf("Matrices' dimensions don't match\n");
        exit(-4);
    }
    prepareResultFile(resPath, m1->height, m2->width);
    createTaskFile(m2->width);

    pid_t* workersArray = malloc(workers*sizeof(int));
    size_t colsPerWorkers = m2->width/workers;
    for(size_t i = 0; i < workers; i++) {
        pid_t worker = fork();
        if(worker == 0) {
            int a = multiplyCols(m1, m2, resPath, i*colsPerWorkers, colsPerWorkers, seconds, true);
            exit(a);
        } else {
            workersArray[i] = worker;
        }
    }

    int status;
    for(size_t i = 0; i < workers; i++) {
        waitpid(workersArray[i], &status, 0);
        printf("Proces %d wykonal %d mnozen macierzy\n", workersArray[i], WEXITSTATUS(status));
    }

    free(workersArray);
    system("rm -f tasks.txt");
}


/** Separate section */
void pasteColumns(size_t colsCount, char* resPath) {
    size_t maxNumLen = (size_t)log(colsCount)+1;
    size_t commandLen = 5 + (18+maxNumLen)*colsCount + 3 + strlen(resPath);
    char* command = (char*) calloc(commandLen, sizeof(char));
    strcpy(command, "paste");

    char* buffer = malloc((18+maxNumLen)*sizeof(char));
    for(size_t i = 0; i < colsCount; i++) {
        sprintf(buffer, "%s%zu%s", " ./columns/col", i, ".txt");
        strcat(command, buffer);
    }
    strcat(command, " > ");
    strcat(command, resPath);
    system(command);

    free(buffer);
    free(command);
}

void multiplySeparate(FileMatrix* m1, FileMatrix* m2, char* resPath, int workers, int seconds) {
    if(m1->width != m2->height) {
        printf("Matrices' dimensions don't match\n");
        exit(-4);
    }
    // We have to clear result file
    FILE* f = fopen(resPath, "w");
    fclose(f);
    createTaskFile(m2->width);
    DIR* dir = opendir("columns");
    if(!dir && ENOENT == errno) {
        system("mkdir columns");
    }
    closedir(dir);

    pid_t* workersArray = malloc(workers*sizeof(pid_t));
    size_t colsPerWorkers = m2->width/workers;
    for(size_t i = 0; i < workers; i++) {
        pid_t worker = fork();
        if(worker == 0) {
            int a = multiplyCols(m1, m2, resPath, i*colsPerWorkers, colsPerWorkers, seconds, false);
            exit(a);
        } else {
            workersArray[i] = worker;
        }
    }

    int status;
    for(size_t i = 0; i < workers; i++) {
        waitpid(workersArray[i], &status, 0);
        printf("Proces %d wykonal %d mnozen macierzy\n", workersArray[i], WEXITSTATUS(status));
    }

    free(workersArray);
    pasteColumns(m2->width, resPath);
    system("rm -rf columns");
    system("rm -f tasks.txt");
}

/** Main matrix function */
void multiplyMatrices(char *filename1, char *filename2, char *resFilename, int workersCount, int seconds, bool shared) {

    FileMatrix* m1 = createMatrixFromFile(filename1);
    FileMatrix* m2 = createMatrixFromFile(filename2);
    if(shared) {
        multiplyShared(m1, m2, resFilename, workersCount, seconds);
    } else {
        multiplySeparate(m1, m2, resFilename, workersCount, seconds);
    }

    freeFileMatrix(m1);
    freeFileMatrix(m2);

}

void multiplyWholeList(char* fileList, int workersCount, int seconds, bool shared) {
    char* filename1 = malloc(MAX_FILENAME_LEN*sizeof(char));
    char* filename2 = malloc(MAX_FILENAME_LEN*sizeof(char));
    char* resFilename = malloc(MAX_FILENAME_LEN*sizeof(char));
    FILE* list = fopen(fileList, "r");

    size_t lines = amountOfLines(list);

    for(size_t i = 0; i < lines; i++) {
        fscanf(list, "%s", filename1);
        fscanf(list, "%s", filename2);
        fscanf(list, "%s", resFilename);
        multiplyMatrices(filename1, filename2, resFilename, workersCount, seconds, shared);
        printf("Multiplied %zu. matrix\n", i+1);
    }

    free(filename1);
    free(filename2);
    free(resFilename);
    fclose(list);
}