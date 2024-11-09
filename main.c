#include <stdio.h>
#include <stdlib.h>
#include "vcs.h"

int main() {
    initRepository();

    const char *filename = "testfile.txt";

    add(filename);

    return 0;
}
