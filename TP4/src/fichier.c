#include "fichier.h"

#include <stdio.h>

int lire_fichier(const char *nom_de_fichier) {
    FILE *fichier = fopen(nom_de_fichier, "r");
    if (fichier == NULL) {
        perror(nom_de_fichier);
        return -1;
    }

    printf("Contenu du fichier %s :\n", nom_de_fichier);
    int caractere;
    while ((caractere = fgetc(fichier)) != EOF) {
        if (putchar(caractere) == EOF) {
            perror("Erreur d'affichage");
            fclose(fichier);
            return -1;
        }
    }
    if (ferror(fichier)) {
        perror(nom_de_fichier);
        fclose(fichier);
        return -1;
    }
    if (fclose(fichier) != 0) {
        perror(nom_de_fichier);
        return -1;
    }
    return 0;
}

int ecrire_dans_fichier(const char *nom_de_fichier, const char *message) {
    FILE *fichier = fopen(nom_de_fichier, "a");
    if (fichier == NULL) {
        perror(nom_de_fichier);
        return -1;
    }
    if (fputs(message, fichier) == EOF || fputc('\n', fichier) == EOF) {
        perror(nom_de_fichier);
        fclose(fichier);
        return -1;
    }
    if (fclose(fichier) != 0) {
        perror(nom_de_fichier);
        return -1;
    }
    printf("Le message a été écrit dans le fichier %s.\n", nom_de_fichier);
    return 0;
}