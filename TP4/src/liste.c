#include "liste.h"

#include <stdio.h>
#include <stdlib.h>

struct noeud_couleur {
    struct couleur valeur;
    struct noeud_couleur *suivant;
};

void init_liste(struct liste_couleurs *liste) {
    liste->tete = NULL;
}

int insertion(const struct couleur *couleur, struct liste_couleurs *liste) {
    struct noeud_couleur *noeud = malloc(sizeof *noeud);
    if (noeud == NULL) {
        return -1;
    }
    noeud->valeur = *couleur;
    noeud->suivant = NULL;

    struct noeud_couleur **position = &liste->tete;
    while (*position != NULL) {
        position = &(*position)->suivant;
    }
    *position = noeud;
    return 0;
}

void parcours(const struct liste_couleurs *liste) {
    for (const struct noeud_couleur *noeud = liste->tete;
         noeud != NULL; noeud = noeud->suivant) {
        printf("#%02x%02x%02x%02x\n",
               (unsigned int)noeud->valeur.rouge,
               (unsigned int)noeud->valeur.vert,
               (unsigned int)noeud->valeur.bleu,
               (unsigned int)noeud->valeur.alpha);
    }
}

void detruire_liste(struct liste_couleurs *liste) {
    struct noeud_couleur *noeud = liste->tete;
    while (noeud != NULL) {
        struct noeud_couleur *suivant = noeud->suivant;
        free(noeud);
        noeud = suivant;
    }
    liste->tete = NULL;
}