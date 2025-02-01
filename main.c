#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1000
#define STRS_ARR_SIZE 50
#define STR_MAX_LEN 10

#undef RAND_MAX
#define RAND_MAX 1000

const char **parse_strs() {
    FILE *fp = fopen("strs.txt", "r");
    if (fp == NULL) {
        printf("%s\n", strerror(errno));
        exit(1337);
    }

    size_t file_sz = (STR_MAX_LEN + 1) * STRS_ARR_SIZE;
    char *buf = malloc(
        (sizeof(char) * file_sz) +
        1); // DON'T FREE IT SINCE strtok() GIVES A INTERNAL POINTER TO THIS
    int bytes_read = fread(buf, sizeof(char), file_sz, fp);
    if (bytes_read == -1) {
        printf("could not read bytes: %s\n", strerror(errno));
        exit(1);
    }
    buf[bytes_read] = '\0';

    const char **strs = malloc(sizeof(const char *) * STRS_ARR_SIZE);
    size_t idx = 0;
    const char *s = strtok(buf, "\n");
    assert(s != NULL);
    strs[idx++] = s;

    while (s != NULL) {
        s = strtok(NULL, "\n");
        strs[idx++] = s;
    }

    return strs;
}

int hash(const char *str) {
    printf("hash for: %s => ", str);
    fflush(stdout);
    unsigned long hs = 5381;

    assert(str != NULL && "Hash key is fking null!!");
    while (*str) {
        char c = *str;

        hs = ((hs << 5) + hs) + c;

        str++;
    }

    printf("%lu [%lu]\n", hs, hs % TABLE_SIZE);
    hs %= TABLE_SIZE; // limit hash to table size

    assert(hs < TABLE_SIZE && "Hash is greater than table size");

    return hs;
}

// Using murmur3d for *near* perfect hashing (gracefully stolen from wikipidia)
uint32_t pr_hash(const uint8_t *key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    uint32_t c1 = 0xcc9e2d51, c2 = 0x1b873593, r1 = 15, r2 = 13, m = 5, n = 0xe6546b64;
    for (size_t i = 0; i < len; i++) {
        uint32_t k = key[i];
        k *= c1; k = (k << r1) | (k >> (32 - r1)); k *= c2;
        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }
    h ^= len;
    h ^= (h >> 16); h *= 0x85ebca6b;
    h ^= (h >> 13); h *= 0xc2b2ae35;
    h ^= (h >> 16);
    return h % 1000;
}

int main() {
    int *table = malloc(sizeof(int) * TABLE_SIZE);
    for (size_t i = 0; i < TABLE_SIZE; i++) {
        table[i] = -1;
    }

    const char *const *strs = parse_strs();
    for (size_t i = 0; i < STRS_ARR_SIZE; i++) {
        const char *key = strs[i];

        const int value = rand();

        const int hashed = pr_hash((uint8_t*)key, strlen(key), rand());

        if (table[hashed] != -1) {
            printf("Collision occured after %zu keys [hash: %d] [key: %s] "
                   "[value stored at collision: %d]\n",
                   i + 1, hashed, key, table[hashed]);
            break;
        }

        table[hashed] = value;

        if (i == STRS_ARR_SIZE - 1) { // last iteration
            printf("%s\n", "Perfect Hash!");
        }
    }

    return 0;
}
