#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *lfrs_obfs(const char *str, const int len, int seed) {
    int lsb = 0;
    char *obfs = malloc(sizeof(char) * (len + 1));

    for (int i = 0; i < len; i++) {
        obfs[i] = str[i] ^ (seed & 0x0ff); // truncating the seed down to 8 bits since char is also 8 bits
        lsb = seed & 1;
        seed = seed >> 1;
        if (lsb) { // since we using the compact method, we have to only do so if lsb is 1
            seed ^= 0xB400u; // this number is basically a number with 16, 14, 13 and 11 bit as 1
        }
    }
    obfs[len] = '\0';

    return obfs;
}

int main() {
    const char *msg = "Good morning";
    const char* obfs = lfrs_obfs(msg, strlen(msg), 0xACE1u);
    printf("Original: %s\n", msg);

    printf("Obfuscated:");
    for (int i = 0; i < strlen(obfs); i++) {
        printf(" %02hhx", obfs[i]);
    }
    printf("\n");

    const char* back = lfrs_obfs(obfs, strlen(obfs), 0xACE1u);
    printf("Back to original: %s\n", back);
}
