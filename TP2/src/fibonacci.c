#include <limits.h>
#include <stdio.h>

int main(void) {
    unsigned int n;
    unsigned long long precedent = 0;
    unsigned long long courant = 1;

    printf("Entrez n : ");
    if (scanf("%u", &n) != 1) {
        fprintf(stderr, "Veuillez entrer un entier non negatif.\n");
        return 1;
    }

    printf("Suite de Fibonacci jusqu'a U%u : ", n);
    for (unsigned long long i = 0; i <= n; ++i) {
        if (i == 0) {
            printf("0");
        } else {
            printf(", %llu", courant);
        }

        if (i > 0 && i < n) {
            if (courant > ULLONG_MAX - precedent) {
                fprintf(stderr, "\nDepassement de capacite avant U%llu.\n",
                        i + 1);
                return 1;
            }
            const unsigned long long suivant = precedent + courant;
            precedent = courant;
            courant = suivant;
        }
    }
    putchar('\n');
    return 0;
}