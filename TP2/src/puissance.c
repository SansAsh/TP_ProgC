#include <stdio.h>

int main(void) {
    // Déclaration et initialisation des variables
    int a = 2;
    int b = 3;
    long long resultat = 1; // "long long" pour éviter les dépassements de capacité

    // Calcul de la puissance (a^b) pour b >= 0
    for (int i = 0; i < b; i++) {
        resultat *= a; // Équivalent à : resultat = resultat * a;
    }

    // Affichage du résultat
    printf("%d élevé à la puissance %d est égal à : %lld\n", a, b, resultat);

    return 0;
}