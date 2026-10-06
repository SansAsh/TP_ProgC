#include "operator.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int convertir_entier(const char *texte, int *valeur) {
    char *fin;
    errno = 0;
    long nombre = strtol(texte, &fin, 10);
    if (errno != 0 || *texte == '\0' || *fin != '\0' ||
        nombre < INT_MIN || nombre > INT_MAX) {
        return -1;
    }
    *valeur = (int)nombre;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4 || argv[1][0] == '\0' || argv[1][1] != '\0') {
        fprintf(stderr, "Usage : %s <opérateur> <num1> <num2>\n", argv[0]);
        return 1;
    }
    int num1;
    int num2;
    if (convertir_entier(argv[2], &num1) != 0 ||
        convertir_entier(argv[3], &num2) != 0) {
        fprintf(stderr, "Les deux opérandes doivent être des entiers valides.\n");
        return 1;
    }

    int resultat;
    switch (argv[1][0]) {
    case '+': resultat = somme(num1, num2); break;
    case '-': resultat = difference(num1, num2); break;
    case '*': resultat = produit(num1, num2); break;
    case '/':
        if (num2 == 0 || (num1 == INT_MIN && num2 == -1)) {
            fprintf(stderr, "Division invalide.\n");
            return 1;
        }
        resultat = quotient(num1, num2);
        break;
    case '%':
        if (num2 == 0 || (num1 == INT_MIN && num2 == -1)) {
            fprintf(stderr, "Modulo invalide.\n");
            return 1;
        }
        resultat = modulo(num1, num2);
        break;
    case '&': resultat = et(num1, num2); break;
    case '|': resultat = ou(num1, num2); break;
    case '~': resultat = negation(num1, num2); break;
    default:
        fprintf(stderr, "Opérateur non pris en charge.\n");
        return 1;
    }
    printf("Résultat : %d\n", resultat);
    return 0;
}