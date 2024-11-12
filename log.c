#include "vcs.h"

void commitLog(commitList* commitList) {
    Commit* curr_commit = *commitList;
    while (curr_commit) {
        char buffer[80];
        struct tm* time_info = localtime(&curr_commit->timestamp);
        printf("Commit: %s\n\n", asctime(time_info));
        curr_commit = curr_commit->parent;
    }
}
