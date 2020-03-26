#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void createRandomMatrix(int height, int width, char* filename) {
    FILE* f = fopen(filename, "w");
    int val;
    for(size_t i = 0; i < height; i++) {
        for(size_t j = 0; j < width; j++) {
            val = rand()%201 - 100;
            if(i == height-1 && j == width-1) {
                fprintf(f, "%d", val);
            } else if( j == width-1) {
                fprintf(f, "%d\n", val);
            } else {
                fprintf(f, "%d ", val);
            }
        }
    }
    fclose(f);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    if(argc < 4) {
        printf("You need to provide amount of wanted file pairs, min and max\n");
        exit(0);
    }
    int filePairsCount = atoi(argv[1]);
    if(filePairsCount <= 0) {
        printf("Amount of file pairs has to be positive\n");
        exit(0);
    }
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);
    if(min > max) {
        printf("Min cannot be higher than max\n");
        exit(0);
    }
    if(min <= 0) {
        printf("Matrix dimension has to be positive\n");
        exit(0);
    }

    DIR* dir = opendir("matrices");
    if(!dir && ENOENT == errno) {
        system("mkdir matrices");
    }
    closedir(dir);
    dir = opendir("results");
    if(!dir && ENOENT == errno) {
        system("mkdir results");
    }
    closedir(dir);

    FILE* f = fopen("lista.txt", "w");
    //int fd = open("lista.txt", O_WRONLY | O_CREAT | O_TRUNC);

    size_t numMaxLen = (size_t)log10(filePairsCount)+1;
    char* filename1 = malloc((24+numMaxLen)*sizeof(char));
    char* filename2 = malloc((24+numMaxLen)*sizeof(char));
    char* resFilename = malloc((18+numMaxLen)*sizeof(char));

    for(int i = 0; i < filePairsCount; i++) {
        int height1 = rand()%(max-min+1) + min;
        int width1 = rand()%(max-min+1) + min;
        sprintf(filename1, "%s%d%s", "./matrices/matrix", i, "a.txt");
        int height2 = width1;
        int width2 = rand()%(max-min+1) + min;
        sprintf(filename2, "%s%d%s", "./matrices/matrix", i, "b.txt");
        sprintf(resFilename, "%s%d%s", "./results/res", i, ".txt");
        createRandomMatrix(height1, width1, filename1);
        createRandomMatrix(height2, width2, filename2);

        //write(fd, filename1, )
        fwrite(filename1, strlen(filename1), sizeof(char), f);
        fwrite(" ", 1, sizeof(char), f);
        fwrite(filename2, strlen(filename2), sizeof(char), f);
        fwrite(" ", 1, sizeof(char), f);
        fwrite(resFilename, strlen(resFilename), sizeof(char), f);
        if(i != filePairsCount-1) {
            fwrite("\n", 1, sizeof(char), f);
        }
    }
    free(filename1);
    free(filename2);
    free(resFilename);

    fclose(f);
    return 0;
}