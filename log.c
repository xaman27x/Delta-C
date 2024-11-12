#include "vcs.h"

void commitLog(commitList* commitList) {
    Commit* curr_commit = *commitList;
    while (curr_commit) {
        char buffer[80];
        struct tm* time_info = localtime(&curr_commit->timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);
        printf("commit: %s\n\n", buffer);
        curr_commit = curr_commit->next;
    }
}
