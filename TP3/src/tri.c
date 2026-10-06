#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 100

static void afficher_tableau(const int tableau[], size_t taille) {
    for (size_t i = 0; i < taille; ++i) {
        printf("%d%s", tableau[i], i + 1 == taille ? "\n" : " ");
    }
}

int main(void) {
    int nombres[TAILLE];

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < TAILLE; ++i) {
        nombres[i] = rand() % 2001 - 1000;
    }

    printf("Tableau non trié :\n");
    afficher_tableau(nombres, TAILLE);

    for (size_t i = 1; i < TAILLE; ++i) {
        const int valeur = nombres[i];
        size_t position = i;
        while (position > 0 && nombres[position - 1] > valeur) {
            nombres[position] = nombres[position - 1];
            --position;
        }
        nombres[position] = valeur;
    }

    printf("Tableau trié par ordre croissant :\n");
    afficher_tableau(nombres, TAILLE);
    return 0;
}