#include "fichier.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOMBRE_ETUDIANTS 5
#define TAILLE_NOM 64
#define TAILLE_ADRESSE 256

struct Etudiant {
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    char adresse[TAILLE_ADRESSE];
    float note1;
    float note2;
};

static int lire_texte(const char *invite, char *destination, size_t capacite) {
    printf("%s", invite);
    if (fgets(destination, (int)capacite, stdin) == NULL) {
        return -1;
    }
    size_t longueur = strlen(destination);
    if (longueur > 0 && destination[longueur - 1] == '\n') {
        destination[longueur - 1] = '\0';
        return 0;
    }
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
    fprintf(stderr, "La saisie est trop longue.\n");
    return -1;
}

static int lire_note(const char *invite, float *note) {
    char entree[64];
    char *fin;
    if (lire_texte(invite, entree, sizeof entree) != 0) {
        return -1;
    }
    errno = 0;
    float valeur = strtof(entree, &fin);
    if (errno != 0 || fin == entree || *fin != '\0') {
        fprintf(stderr, "Note invalide.\n");
        return -1;
    }
    *note = valeur;
    return 0;
}

int main(void) {
    struct Etudiant etudiants[NOMBRE_ETUDIANTS];
    for (size_t i = 0; i < NOMBRE_ETUDIANTS; ++i) {
        printf("Entrez les détails de l'étudiant.e %zu :\n", i + 1);
        if (lire_texte("Nom : ", etudiants[i].nom,
                       sizeof etudiants[i].nom) != 0 ||
            lire_texte("Prénom : ", etudiants[i].prenom,
                       sizeof etudiants[i].prenom) != 0 ||
            lire_texte("Adresse : ", etudiants[i].adresse,
                       sizeof etudiants[i].adresse) != 0 ||
            lire_note("Note 1 : ", &etudiants[i].note1) != 0 ||
            lire_note("Note 2 : ", &etudiants[i].note2) != 0) {
            return 1;
        }

        char ligne[768];
        int longueur = snprintf(ligne, sizeof ligne,
                                "%s;%s;%s;%.2f;%.2f",
                                etudiants[i].nom, etudiants[i].prenom,
                                etudiants[i].adresse, etudiants[i].note1,
                                etudiants[i].note2);
        if (longueur < 0 || (size_t)longueur >= sizeof ligne ||
            ecrire_dans_fichier("etudiant.txt", ligne) != 0) {
            fprintf(stderr, "Impossible d'enregistrer l'étudiant.\n");
            return 1;
        }
    }
    printf("Les détails des étudiants ont été enregistrés dans "
           "le fichier etudiant.txt.\n");
    return 0;
}