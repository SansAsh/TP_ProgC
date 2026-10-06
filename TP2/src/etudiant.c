#include <stdio.h>

int main(void) {
    const char *noms[5] = {
        "Dupont", "Martin", "Bernard", "Petit", "Robert"
    };
    const char *prenoms[5] = {
        "Marie", "Pierre", "Sofia", "Lucas", "Emma"
    };
    const char *adresses[5] = {
        "20, Boulevard Niels Bohr, Lyon",
        "22, Boulevard Niels Bohr, Lyon",
        "5, rue de la Paix, Lyon",
        "14, avenue des Sciences, Lyon",
        "8, rue Victor Hugo, Lyon"
    };
    const float notes_programmation[5] = {16.5f, 14.0f, 17.2f, 12.8f, 15.0f};
    const float notes_systeme[5] = {12.1f, 14.1f, 15.5f, 13.0f, 18.0f};

    for (size_t i = 0; i < 5; ++i) {
        printf("Étudiant.e %zu :\n", i + 1);
        printf("Nom : %s\n", noms[i]);
        printf("Prénom : %s\n", prenoms[i]);
        printf("Adresse : %s\n", adresses[i]);
        printf("Note en programmation C : %.1f\n", notes_programmation[i]);
        printf("Note en système d'exploitation : %.1f\n\n", notes_systeme[i]);
    }
    return 0;
}