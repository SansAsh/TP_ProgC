#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned long long factorielle(unsigned int n) {
    return n < 2 ? 1ULL : (unsigned long long)n * factorielle(n - 1);
}

int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Usage : %s [n]\n", argv[0]);
        return 1;
    }
    unsigned int maximum = 10;
    if (argc == 2) {
        char *fin;
        errno = 0;
        unsigned long nombre = strtoul(argv[1], &fin, 10);
        if (errno != 0 || *argv[1] == '\0' || *fin != '\0' ||
            nombre > 20) {
            fprintf(stderr, "n doit être compris entre 0 et 20.\n");
            return 1;
        }
        maximum = (unsigned int)nombre;
    }
    if (ULLONG_MAX < 2432902008176640000ULL) {
        fprintf(stderr, "Le type unsigned long long est trop petit.\n");
        return 1;
    }
    for (unsigned int i = 0; i <= maximum; ++i) {
        printf("%u! = %llu\n", i, factorielle(i));
    }
    return 0;
}