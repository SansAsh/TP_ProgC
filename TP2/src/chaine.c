#include <stdio.h>

static size_t longueur_chaine(const char *chaine) {
    size_t longueur = 0;

    while (chaine[longueur] != '\0') {
        ++longueur;
    }
    return longueur;
}

static int copier_chaine(char *destination, size_t capacite,
                         const char *source) {
    size_t i = 0;

    while (source[i] != '\0' && i + 1 < capacite) {
        destination[i] = source[i];
        ++i;
    }
    if (source[i] != '\0' || capacite == 0) {
        return 0;
    }
    destination[i] = '\0';
    return 1;
}

static int concatener_chaine(char *destination, size_t capacite,
                             const char *source) {
    const size_t longueur_destination = longueur_chaine(destination);
    size_t i = 0;

    while (source[i] != '\0' && longueur_destination + i + 1 < capacite) {
        destination[longueur_destination + i] = source[i];
        ++i;
    }
    if (source[i] != '\0' ||
        longueur_destination + i + 1 > capacite) {
        return 0;
    }
    destination[longueur_destination + i] = '\0';
    return 1;
}

int main(void) {
    const char premiere[] = "Hello";
    const char seconde[] = " World!";
    char copie[32];
    char concatenee[32];

    if (!copier_chaine(copie, sizeof copie, premiere) ||
        !copier_chaine(concatenee, sizeof concatenee, premiere) ||
        !concatener_chaine(concatenee, sizeof concatenee, seconde)) {
        fprintf(stderr, "Capacite insuffisante pour manipuler les chaines.\n");
        return 1;
    }

    printf("Longueur de la premiere chaine : %zu\n",
           longueur_chaine(premiere));
    printf("Longueur de la seconde chaine : %zu\n",
           longueur_chaine(seconde));
    printf("Longueur totale : %zu\n", longueur_chaine(concatenee));
    printf("Copie : %s\n", copie);
    printf("Concaténation : %s\n", concatenee);
    return 0;
}