#define _XOPEN_SOURCE 700
#include "repertoire.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int chemin_enfant(char *destination, size_t capacite,
                         const char *parent, const char *nom) {
    int longueur = snprintf(destination, capacite, "%s/%s", parent, nom);
    if (longueur < 0 || (size_t)longueur >= capacite) {
        errno = ENAMETOOLONG;
        return -1;
    }
    return 0;
}

int lire_dossier(const char *nom_repertoire) {
    DIR *dossier = opendir(nom_repertoire);
    if (dossier == NULL) {
        perror(nom_repertoire);
        return -1;
    }

    int resultat = 0;
    struct dirent *entree;
    errno = 0;
    while ((entree = readdir(dossier)) != NULL) {
        if (strcmp(entree->d_name, ".") != 0 &&
            strcmp(entree->d_name, "..") != 0) {
            puts(entree->d_name);
        }
        errno = 0;
    }
    if (errno != 0) {
        perror(nom_repertoire);
        resultat = -1;
    }
    if (closedir(dossier) != 0) {
        perror(nom_repertoire);
        resultat = -1;
    }
    return resultat;
}

static int parcourir_recursif(const char *chemin, unsigned int profondeur) {
    DIR *dossier = opendir(chemin);
    if (dossier == NULL) {
        perror(chemin);
        return -1;
    }
    int resultat = 0;
    struct dirent *entree;
    errno = 0;
    while ((entree = readdir(dossier)) != NULL) {
        if (strcmp(entree->d_name, ".") == 0 ||
            strcmp(entree->d_name, "..") == 0) {
            errno = 0;
            continue;
        }
        char enfant[PATH_MAX];
        if (chemin_enfant(enfant, sizeof enfant, chemin, entree->d_name) != 0) {
            perror("Chemin trop long");
            resultat = -1;
            errno = 0;
            continue;
        }
        printf("%*s%s\n", (int)(profondeur * 2), "", enfant);
        struct stat informations;
        if (lstat(enfant, &informations) != 0) {
            perror(enfant);
            resultat = -1;
        } else if (S_ISDIR(informations.st_mode) &&
                   parcourir_recursif(enfant, profondeur + 1) != 0) {
            resultat = -1;
        }
        errno = 0;
    }
    if (errno != 0) {
        perror(chemin);
        resultat = -1;
    }
    if (closedir(dossier) != 0) {
        perror(chemin);
        resultat = -1;
    }
    return resultat;
}

int lire_dossier_recursif(const char *nom_repertoire) {
    return parcourir_recursif(nom_repertoire, 0);
}

static int empiler(char ***pile, size_t *taille, size_t *capacite,
                   const char *chemin) {
    if (*taille == *capacite) {
        size_t nouvelle_capacite = *capacite == 0 ? 16 : *capacite * 2;
        char **nouvelle_pile = realloc(*pile,
                                       nouvelle_capacite * sizeof **pile);
        if (nouvelle_pile == NULL) {
            return -1;
        }
        *pile = nouvelle_pile;
        *capacite = nouvelle_capacite;
    }
    (*pile)[*taille] = malloc(strlen(chemin) + 1);
    if ((*pile)[*taille] == NULL) {
        return -1;
    }
    strcpy((*pile)[*taille], chemin);
    ++*taille;
    return 0;
}

int lire_dossier_iteratif(const char *nom_repertoire) {
    char **pile = NULL;
    size_t taille = 0;
    size_t capacite = 0;
    int resultat = 0;

    if (empiler(&pile, &taille, &capacite, nom_repertoire) != 0) {
        perror("Allocation de la pile");
        return -1;
    }
    while (taille > 0) {
        char *chemin = pile[--taille];
        DIR *dossier = opendir(chemin);
        if (dossier == NULL) {
            perror(chemin);
            resultat = -1;
            free(chemin);
            continue;
        }
        struct dirent *entree;
        errno = 0;
        while ((entree = readdir(dossier)) != NULL) {
            if (strcmp(entree->d_name, ".") == 0 ||
                strcmp(entree->d_name, "..") == 0) {
                errno = 0;
                continue;
            }
            char enfant[PATH_MAX];
            if (chemin_enfant(enfant, sizeof enfant, chemin,
                              entree->d_name) != 0) {
                perror("Chemin trop long");
                resultat = -1;
                errno = 0;
                continue;
            }
            puts(enfant);
            struct stat informations;
            if (lstat(enfant, &informations) != 0) {
                perror(enfant);
                resultat = -1;
            } else if (S_ISDIR(informations.st_mode) &&
                       empiler(&pile, &taille, &capacite, enfant) != 0) {
                perror("Allocation de la pile");
                resultat = -1;
            }
            errno = 0;
        }
        if (errno != 0) {
            perror(chemin);
            resultat = -1;
        }
        if (closedir(dossier) != 0) {
            perror(chemin);
            resultat = -1;
        }
        free(chemin);
    }
    free(pile);
    return resultat;
}