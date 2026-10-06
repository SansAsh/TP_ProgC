#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 11

int main(void) {
    int entiers[TAILLE];
    float reels[TAILLE];
    int *p_entier = entiers;
    float *p_reel = reels;

    srand((unsigned int)time(NULL));
    for (int i = 0; i < TAILLE; ++i, ++p_entier, ++p_reel) {
        *p_entier = rand() % 200;
        *p_reel = (float)(rand() % 1000) / 100.0f;
    }

    printf("Tableau d'entiers avant multiplication par 3 :\n");
    for (p_entier = entiers; p_entier < entiers + TAILLE; ++p_entier) {
        printf("%d%s", *p_entier,
               p_entier + 1 == entiers + TAILLE ? "\n" : ", ");
    }
    printf("Tableau de reels avant multiplication par 3 :\n");
    for (p_reel = reels; p_reel < reels + TAILLE; ++p_reel) {
        printf("%.2f%s", *p_reel,
               p_reel + 1 == reels + TAILLE ? "\n" : ", ");
    }

    p_entier = entiers;
    p_reel = reels;
    for (int indice = 0; indice < TAILLE;
         ++indice, ++p_entier, ++p_reel) {
        if (indice % 2 == 0) {
            *p_entier *= 3;
            *p_reel *= 3.0f;
        }
    }

    printf("Tableau d'entiers apres multiplication par 3 :\n");
    for (p_entier = entiers; p_entier < entiers + TAILLE; ++p_entier) {
        printf("%d%s", *p_entier,
               p_entier + 1 == entiers + TAILLE ? "\n" : ", ");
    }
    printf("Tableau de reels apres multiplication par 3 :\n");
    for (p_reel = reels; p_reel < reels + TAILLE; ++p_reel) {
        printf("%.2f%s", *p_reel,
               p_reel + 1 == reels + TAILLE ? "\n" : ", ");
    }
    return 0;
}