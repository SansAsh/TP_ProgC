
#include "fichier.h"
#include "liste.h"
#include "operator.h"

#include <stdio.h>
#include <string.h>

static int lire_entier(const char *invite, int *valeur) {
    printf("%s", invite);
    return scanf("%d", valeur) == 1 ? 0 : -1;
}

static void exercice_operateurs(void) {
    int num1;
    int num2;
    char operateur;

    if (lire_entier("Entrez num1 : ", &num1) != 0 ||
        lire_entier("Entrez num2 : ", &num2) != 0) {
        fprintf(stderr, "Entree numerique invalide.\n");
        return;
    }
    printf("Entrez l'operateur (+, -, *, /, %%, &, |, ~) : ");
    if (scanf(" %c", &operateur) != 1) {
        fprintf(stderr, "Operateur invalide.\n");
        return;
    }

    int resultat;
    switch (operateur) {
    case '+': resultat = somme(num1, num2); break;
    case '-': resultat = difference(num1, num2); break;
    case '*': resultat = produit(num1, num2); break;
    case '/':
        if (num2 == 0) {
            fprintf(stderr, "Division par zero impossible.\n");
            return;
        }
        resultat = quotient(num1, num2);
        break;
    case '%':
        if (num2 == 0) {
            fprintf(stderr, "Modulo par zero impossible.\n");
            return;
        }
        resultat = modulo(num1, num2);
        break;
    case '&': resultat = et(num1, num2); break;
    case '|': resultat = ou(num1, num2); break;
    case '~': resultat = negation(num1, num2); break;
    default:
        fprintf(stderr, "Operateur non pris en charge.\n");
        return;
    }
    printf("Résultat : %d\n", resultat);
}

static int lire_ligne(const char *invite, char *buffer, size_t capacite) {
    printf("%s", invite);
    if (fgets(buffer, (int)capacite, stdin) == NULL) {
        return -1;
    }
    size_t longueur = strlen(buffer);
    if (longueur > 0 && buffer[longueur - 1] == '\n') {
        buffer[longueur - 1] = '\0';
    } else {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
    }
    return 0;
}

static void exercice_fichiers(void) {
    int choix;
    char nom[256];
    char message[1024];

    printf("1. Lire un fichier\n2. Écrire dans un fichier\nVotre choix : ");
    if (scanf("%d", &choix) != 1) {
        fprintf(stderr, "Choix invalide.\n");
        return;
    }
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }

    if (lire_ligne(choix == 1 ? "Entrez le nom du fichier à lire : "
                              : "Entrez le nom du fichier à écrire : ",
                   nom, sizeof nom) != 0) {
        fprintf(stderr, "Impossible de lire le nom de fichier.\n");
        return;
    }
    if (choix == 1) {
        (void)lire_fichier(nom);
    } else if (choix == 2) {
        if (lire_ligne("Entrez le message à écrire : ", message,
                       sizeof message) != 0) {
            fprintf(stderr, "Impossible de lire le message.\n");
            return;
        }
        (void)ecrire_dans_fichier(nom, message);
    } else {
        fprintf(stderr, "Choix inconnu.\n");
    }
}

static void exercice_liste(void) {
    const struct couleur couleurs[10] = {
        {0xff, 0x00, 0x00, 0xff}, {0x00, 0xff, 0x00, 0xff},
        {0x00, 0x00, 0xff, 0xff}, {0xff, 0xff, 0x00, 0xff},
        {0xff, 0x00, 0xff, 0xff}, {0x00, 0xff, 0xff, 0xff},
        {0x80, 0x80, 0x80, 0xff}, {0xff, 0x80, 0x00, 0xff},
        {0x80, 0x00, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}
    };
    struct liste_couleurs liste;
    init_liste(&liste);
    for (size_t i = 0; i < sizeof couleurs / sizeof couleurs[0]; ++i) {
        if (insertion(&couleurs[i], &liste) != 0) {
            fprintf(stderr, "Impossible d'allouer un noeud de couleur.\n");
            detruire_liste(&liste);
            return;
        }
    }
    printf("Liste des couleurs :\n");
    parcours(&liste);
    detruire_liste(&liste);
}

int main(void) {
    int choix;
    printf("Choisissez un exercice :\n");
    printf("1. Calcul avec opérateurs\n2. Gestion de fichiers\n");
    printf("7. Liste de couleurs\nVotre choix : ");
    if (scanf("%d", &choix) != 1) {
        fprintf(stderr, "Choix invalide.\n");
        return 1;
    }
    switch (choix) {
    case 1: exercice_operateurs(); break;
    case 2: exercice_fichiers(); break;
    case 7: exercice_liste(); break;
    default:
        fprintf(stderr, "Exercice inconnu.\n");
        return 1;
    }
    return 0;
}
