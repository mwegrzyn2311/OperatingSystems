#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <ftw.h>
#include <unistd.h>
#include <sys/wait.h>

bool usingNftw = false;
bool checkDepth = false;
int maxDepth = -1;
bool checkMTime = false;
int aTimeMode=-2;
int wantedATime;
bool checkATime = false;
int mTimeMode=-2;
int wantedMTime;

int getDays(time_t time) {
    struct tm* t;
    t = localtime(&time);
    int res = t->tm_mday + 30*t->tm_mon + 365*t->tm_year;
    return res;
}

bool fitsInTime(struct stat* s) {
    if(checkATime) {
        int current = getDays(time(NULL));
        int stat = getDays(s->st_atime);
        if(aTimeMode == 1 && current - stat < wantedATime)
            return false;
        if(aTimeMode == -1 && current - stat > wantedATime)
            return false;
        if(aTimeMode == 0 && current - stat != wantedATime)
            return false;
    }
    if(checkMTime) {
        int current = getDays(time(NULL));
        int stat = getDays(s->st_mtime);
        if(mTimeMode == 1 && current - stat < wantedMTime)
            return false;
        if(mTimeMode == -1 && current - stat > wantedMTime)
            return false;
        if(mTimeMode == 0 && current - stat != wantedMTime)
            return false;
    }
    return true;
}

char* getFilePath(char* dir, char* path) {
    char* res = (char*) calloc(strlen(dir) + strlen(path) + 2, sizeof(char));
    strcpy(res, dir);
    strcat(res, "/");
    strcat(res, path);
    return res;
}
void searchDirectory(char* path, int currentDepth){
    if(checkDepth && currentDepth > maxDepth) {
        return;
    }
    DIR* dir = opendir(path);
    if(dir == NULL) {
        return;
    }

    struct dirent* d = readdir(dir);
    struct stat s;
    // As long as stuff exists
    while(d != NULL) {
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || d->d_type != DT_DIR) {
            d = readdir(dir);
            continue;
        }
        // For each file or directory, we start by getting its path
        char* filePath = getFilePath(path, d->d_name);
        // Then we check if we can enter/read it
        if(lstat(filePath, &s) >= 0 && fitsInTime(&s)) {
            // we can already print it
            /*
                printFileInfo(filePath, &s);
            */
            // But if it is a directory, we need to go deeper
            if(S_ISDIR(s.st_mode)) {
                int res = fork();
                if(res == 0) {
                    printf("%s\n", filePath);
                    chdir(filePath);
                    system("ls -l");
                    chdir("..");
                    exit(1);
                } else {
                    wait(0);
                }
                searchDirectory(filePath, currentDepth+1);
            }
        }
        free(filePath);
        // And we proceed with the next file/directory
        d = readdir(dir);
    }
    closedir(dir);
}

void find(char* path) {
    struct stat s;
    // We check if we can read it
    if(lstat(path, &s) >= 0 && fitsInTime(&s) && S_ISDIR(s.st_mode)) {
        int res = fork();
        if(res == 0) {
            printf("%s\n", path);
            chdir(path);
            system("ls -l");
            chdir("..");
            exit(0);
        } else {
            wait(0);
        }
    }
    searchDirectory(path, 1);
}


int execLs(char* path, struct stat* s, int flag, struct FTW* ftw) {
    if(checkDepth && ftw->level > maxDepth) return 0;
    if(!fitsInTime(s)) return 0;
    if(flag == FTW_D) {
        int res = fork();
        if(res == 0) {
            printf("%s\n", path);
            chdir(path);
            system("ls -l");
            chdir("..");
            return 1;
        } else {
            wait(0);
        }
    }
    return 0;
}
void findNftw(char* path) {
    nftw(path, execLs, FTW_D, 0);
}


int main(int argc, char** argv) {
    bool maxDepthSpecified = false;
    bool aTimeCheckSpecified = false;
    bool mTimeCheckSpecified = false;
    char* path;
    int i;
    if(argc == 1 || argv[1][0] == '-') {
        path = ".";
        i = 1;
    } else {
        path = argv[1];
        i = 2;
    }
    while(i < argc) {
        if(!maxDepthSpecified && strcmp(argv[i], "-maxdepth") == 0) {
            maxDepthSpecified = true;
            checkDepth = true;
            i++;
            if(i < argc) {
                maxDepth = atoi(argv[i]);
                i++;
            }
        } else if(!aTimeCheckSpecified && strcmp(argv[i], "-atime") == 0) {
            aTimeCheckSpecified = true;
            checkATime = true;
            i++;
            if(i < argc) {
                if(argv[i][0] == '+'){
                    aTimeMode = 1;
                    char* days = (&argv[i][1]);
                    wantedATime = atoi(days);
                } else if(argv[i][0] == '-') {
                    aTimeMode = -1;
                    char* days = (&argv[i][1]);
                    wantedATime = atoi(days);
                } else {
                    aTimeMode = 0;
                    wantedATime = atoi(argv[i]);
                }
                i++;
            }
        } else if(!mTimeCheckSpecified && strcmp(argv[i], "-mtime") == 0) {
            mTimeCheckSpecified = true;
            checkMTime = true;
            i++;
            if(i < argc) {
                if(argv[i][0] == '+'){
                    mTimeMode = 1;
                    char* days = (&argv[i][1]);
                    wantedMTime = atoi(days);
                } else if(argv[i][0] == '-') {
                    mTimeMode = -1;
                    char* days = (&argv[i][1]);
                    wantedMTime = atoi(days);
                } else {
                    mTimeMode = 0;
                    wantedMTime = atoi(argv[i]);
                }
                i++;
            }
        } else if(usingNftw == false && strcmp(argv[i],"nftw") == 0) {
            usingNftw = true;
            i++;
        }
    }

    if(path[strlen(path)-1] == '/') {
        char *newPath = (char *) calloc(strlen(path) - 1, sizeof(char));
        strncpy(newPath, path, strlen(path) - 1);
        if(usingNftw) {
            findNftw(newPath);
        } else {
            find(newPath);
        }
        free(newPath);
    } else {
        if(usingNftw) {
            findNftw(path);
        } else {
            find(path);
        }
    }
    return 0;
}