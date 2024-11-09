#include "vcs.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHAContext;

void SHATransform(uint32_t state[5], const uint8_t buffer[64]);
void SHAInit(SHAContext* context);
void SHAUpdate(SHAContext* context, const uint8_t* data, uint32_t len);
void SHAFinal(uint8_t digest[SHA_BLOCK_SIZE], SHAContext* context);

#define ROTLEFT(a, b) ((a << b) | (a >> (32 - b)))
#define F0(b, c, d) ((b & c) | (~b & d))
#define F1(b, c, d) (b ^ c ^ d)
#define F2(b, c, d) ((b & c) | (b & d) | (c & d))

void SHATransform(uint32_t state[5], const uint8_t buffer[64]) {
    uint32_t a, b, c, d, e, t, i;
    uint32_t block[80];

    for (i = 0; i < 16; i++) {
        block[i] = (buffer[i * 4] << 24) | (buffer[i * 4 + 1] << 16) | (buffer[i * 4 + 2] << 8) | buffer[i * 4 + 3];
    }
    for (i = 16; i < 80; i++) {
        block[i] = ROTLEFT(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1);
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    for (i = 0; i < 80; i++) {
        if (i < 20) {
            t = F0(b, c, d) + 0x5A827999;
        } else if (i < 40) {
            t = F1(b, c, d) + 0x6ED9EBA1;
        } else if (i < 60) {
            t = F2(b, c, d) + 0x8F1BBCDC;
        } else {
            t = F1(b, c, d) + 0xCA62C1D6;
        }
        t += ROTLEFT(a, 5) + e + block[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = t;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void SHAInit(SHAContext* context) {
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

void SHAUpdate(SHAContext* context, const uint8_t* data, uint32_t len) {
    uint32_t i, j;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        SHATransform(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            SHATransform(context->state, &data[i]);
        }
        j = 0;
    } else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}

void SHAFinal(uint8_t digest[SHA_BLOCK_SIZE], SHAContext* context) {
    uint8_t finalcount[8];
    uint32_t i;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }
    SHAUpdate(context, (const uint8_t*)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHAUpdate(context, (const uint8_t*)"\0", 1);
    }
    SHAUpdate(context, finalcount, 8);
    for (i = 0; i < SHA_BLOCK_SIZE; i++) {
        digest[i] = (uint8_t)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
}

void calculateSHA(const char* input, char output[41]) {
    SHAContext context;
    uint8_t digest[SHA_BLOCK_SIZE];

    SHAInit(&context);
    SHAUpdate(&context, (const uint8_t*)input, strlen(input));
    SHAFinal(digest, &context);

    for (int i = 0; i < SHA_BLOCK_SIZE; i++) {
        sprintf(&output[i * 2], "%02x", digest[i]);
    }
    output[40] = '\0';
    return;
}
