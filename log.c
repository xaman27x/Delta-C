#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include "vcs.h"

void log(commitList* commitList) {
    Commit* curr_commit = *commitList;
    while(curr_commit) {
        char buffer[80]; 
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &curr_commit->timestamp);
        printf("commit: \n\n", curr_commit->timestamp);
        curr_commit = curr_commit->next;
    }
}