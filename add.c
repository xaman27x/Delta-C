#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcs.h"
#include "blob.h"

void add(char* filename) {
    //calculate hash of the file 'filename'
    //check if blob with this hash alrdy exists in .delta/objects
    //if no then create blob object for the file 'filename'
    Blob *newBlob = createBlob(filename);
    //we store the blob in .delta/objects in subdirectory named as the first two characters of the files hash and with a filename as the remaining characters.
    storeBlobObject(blob);
    //we update .delta/index (which is created on first add) by adding an entry which contains filepath, hash(reference to blob), timestamp.
    FILE *index_file = fopen(index, "w");     
    if (index_file == NULL) {
        perror("Error creating file");
        return 1;
    }
    /*add_index_entry();*/
    fclose(index_file);
}