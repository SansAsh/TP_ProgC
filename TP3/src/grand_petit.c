#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 100

int main(void) {
    int nombres[TAILLE];

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < TAILLE; ++i) {
        nombres[i] = rand() % 1000 + 1;
    }

    int minimum = nombres[0];
    int maximum = nombres[0];
    for (size_t i = 1; i < TAILLE; ++i) {
        if (nombres[i] < minimum) {
            minimum = nombres[i];
        }
        if (nombres[i] > maximum) {
            maximum = nombres[i];
        }
    }

    printf("Le numéro le plus grand est : %d\n", maximum);
    printf("Le numéro le plus petit est : %d\n", minimum);
    return 0;
}