#include <stdio.h>

#define TAILLE 100

int main(void) {
    int nombres[TAILLE];

    for (size_t i = 0; i < TAILLE; ++i) {
        nombres[i] = (int)i * 2;
    }

    printf("Tableau trié :\n");
    for (size_t i = 0; i < TAILLE; ++i) {
        printf("%d%s", nombres[i], i + 1 == TAILLE ? "\n" : " ");
    }

    int recherche;
    printf("Entrez l'entier que vous souhaitez chercher : ");
    if (scanf("%d", &recherche) != 1) {
        fprintf(stderr, "Veuillez entrer un entier valide.\n");
        return 1;
    }

    size_t debut = 0;
    size_t fin = TAILLE;
    int present = 0;
    while (debut < fin) {
        const size_t milieu = debut + (fin - debut) / 2;
        if (nombres[milieu] == recherche) {
            present = 1;
            break;
        }
        if (nombres[milieu] < recherche) {
            debut = milieu + 1;
        } else {
            fin = milieu;
        }
    }

    printf("Résultat : entier %s\n", present ? "présent" : "absent");
    return 0;
}