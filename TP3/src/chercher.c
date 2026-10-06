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

    printf("Tableau :\n");
    for (size_t i = 0; i < TAILLE; ++i) {
        printf("%d%s", nombres[i], i + 1 == TAILLE ? "\n" : " ");
    }

    int recherche;
    printf("Entrez l'entier que vous souhaitez chercher : ");
    if (scanf("%d", &recherche) != 1) {
        fprintf(stderr, "Veuillez entrer un entier valide.\n");
        return 1;
    }

    int present = 0;
    for (size_t i = 0; i < TAILLE; ++i) {
        if (nombres[i] == recherche) {
            present = 1;
            break;
        }
    }
    printf("Résultat : entier %s\n", present ? "présent" : "absent");
    return 0;
}