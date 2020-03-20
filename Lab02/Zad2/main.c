#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <ftw.h>

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

char* fileTypeName(struct stat* s) {
    int mode = s->st_mode;
    if (S_ISREG(mode)) return "file";
    if (S_ISDIR(mode)) return "dir";
    if (S_ISCHR(mode)) return "char dev";
    if (S_ISBLK(mode)) return "block dev";
    if (S_ISFIFO(mode)) return "fifo";
    if (S_ISLNK(mode)) return "slink";
    if (S_ISSOCK(mode)) return "sock";
    return "Unexpected type exception";
}

void printFileInfo(char *path, struct stat *s) {
    static const char* timeFormat = "%Y %b %d %T";
    struct tm a, m;
    static char aTime[21];
    static char mTime[21];

    localtime_r(&s->st_atime, &a);
    localtime_r(&s->st_mtime, &m);

    strftime(aTime, 21, timeFormat, &a);
    strftime(mTime, 21, timeFormat, &m);

    printf("%s\t%lu\t%s\t%lu\t%s\t%s\n", path, s->st_nlink, fileTypeName(s), s->st_size, aTime, mTime);
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
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
            d = readdir(dir);
            continue;
        }
        // For each file or directory, we start by getting its path
        char* filePath = getFilePath(path, d->d_name);
        // Then we check if we can enter/read it
        if(lstat(filePath, &s) >= 0 && fitsInTime(&s)) {
            // we can already print it
            printFileInfo(filePath, &s);
            // But if it is a directory, we need to go deeper
            if(S_ISDIR(s.st_mode)) {
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
    if(lstat(path, &s) >= 0 && fitsInTime(&s)) {
        printFileInfo(path, &s);
    }
    searchDirectory(path, 1);
}


int getFiles(char* path, struct stat* s, int flag, struct FTW* ftw) {
    if(checkDepth && ftw->level > maxDepth) return 0;
    if(!fitsInTime(s)) return 0;
    printFileInfo(path, s);
    return 0;
}
void findNftw(char* path) {
    // Problem: nftw currently does not support maxdepth
    nftw(path, getFiles, 0, 0);
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