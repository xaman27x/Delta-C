#include <stdio.h>
#include <string.h>
#define SHA_DIGEST_LENGTH 20

void calculateSHA1(const char* input, char output[41]) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", hash[i]);
    }
    output[40] = '\0';
}
