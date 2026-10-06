#include <limits.h>
#include <stdio.h>

int main(void) {
    unsigned int d = UINT_MAX;
    const unsigned int largeur = (unsigned int)(sizeof d * CHAR_BIT);
    int resultat = 0;

    if (largeur >= 20) {
        const unsigned int bit4 = (d >> (largeur - 4U)) & 1U;
        const unsigned int bit20 = (d >> (largeur - 20U)) & 1U;
        resultat = (bit4 == 1U && bit20 == 1U);
    }

    printf("%d\n", resultat);
    return 0;
}