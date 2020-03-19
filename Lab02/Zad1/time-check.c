#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include "library.h"

double timeDelta(clock_t start, clock_t end){
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void fprintTime(FILE* file, struct tms** tmsTime) {
    fprintf(file, "\tcopy_sys: USER TIME = %g | SYS TIME = %g\n", timeDelta(tmsTime[0]->tms_utime, tmsTime[1]->tms_utime), timeDelta(tmsTime[0]->tms_stime, tmsTime[1]->tms_stime));
    fprintf(file, "\tcopy_lib: USER TIME = %g | SYS TIME = %g\n", timeDelta(tmsTime[2]->tms_utime, tmsTime[3]->tms_utime), timeDelta(tmsTime[2]->tms_stime, tmsTime[3]->tms_stime));
    fprintf(file, "\tsort_sys: USER TIME = %g | SYS TIME = %g\n", timeDelta(tmsTime[1]->tms_utime, tmsTime[2]->tms_utime), timeDelta(tmsTime[1]->tms_stime, tmsTime[2]->tms_stime));
    fprintf(file, "\tsort_lib: USER TIME = %g | SYS TIME = %g\n", timeDelta(tmsTime[3]->tms_utime, tmsTime[4]->tms_utime), timeDelta(tmsTime[3]->tms_stime, tmsTime[4]->tms_stime));
}

int main() {
    int small = 100;
    int big = 1000;
    FILE* wyniki = fopen("wyniki.txt", "w");
    fprintf(wyniki, "Time measurement (of copy and sort operations) results:\n");

    int timeStepsAmount = 5;
    struct tms** tmsTime = (struct tms**) calloc(timeStepsAmount, sizeof(struct tms *));
    for(int i = 0; i < timeStepsAmount; i++) {
        tmsTime[i] = (struct tms*) calloc(1, sizeof(struct tms));
    }
    for(int i = 1; i <= 8192; i*=4) {
        fprintf(wyniki, "\nSIZE = %d\n", i);
        fprintf(wyniki, "Small record counter(%d) and record size of %d\n",small, i);
        generate("dane.txt", small, i);
        times(tmsTime[0]);
        copy_sys("dane.txt", "dane-copy.txt", small, i);
        times(tmsTime[1]);
        sort_sys("dane-copy.txt", small, i);
        times(tmsTime[2]);
        copy_lib("dane.txt", "dane-copy.txt", small, i);
        times(tmsTime[3]);
        sort_lib("dane-copy.txt", small, i);
        times(tmsTime[4]);
        fprintTime(wyniki, tmsTime);

        fprintf(wyniki, "Big record counter(%d) and record size of %d\n",big, i);
        generate("dane.txt", big, i);
        times(tmsTime[0]);
        copy_sys("dane.txt", "dane-copy.txt", big, i);
        times(tmsTime[1]);
        sort_sys("dane-copy.txt", big, i);
        times(tmsTime[2]);
        copy_lib("dane.txt", "dane-copy.txt", big, i);
        times(tmsTime[3]);
        sort_lib("dane-copy.txt", big, i);
        times(tmsTime[4]);
        fprintTime(wyniki, tmsTime);
    }
    for(int i = 0; i < timeStepsAmount; i++) {
        free(tmsTime[i]);
    }
    free(tmsTime);
    fclose(wyniki);
    return 0;
}