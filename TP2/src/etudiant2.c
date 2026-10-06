#include <stdio.h>
#include <string.h>

struct Etudiant {
    char nom[32];
    char prenom[32];
    char adresse[96];
    float note_programmation;
    float note_systeme;
};

int main(void) {
    struct Etudiant etudiants[5] = {0};
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
        strcpy(etudiants[i].nom, noms[i]);
        strcpy(etudiants[i].prenom, prenoms[i]);
        strcpy(etudiants[i].adresse, adresses[i]);
        etudiants[i].note_programmation = notes_programmation[i];
        etudiants[i].note_systeme = notes_systeme[i];
    }

    for (size_t i = 0; i < 5; ++i) {
        printf("Étudiant.e %zu :\n", i + 1);
        printf("Nom : %s\n", etudiants[i].nom);
        printf("Prénom : %s\n", etudiants[i].prenom);
        printf("Adresse : %s\n", etudiants[i].adresse);
        printf("Note en programmation C : %.1f\n",
               etudiants[i].note_programmation);
        printf("Note en système d'exploitation : %.1f\n\n",
               etudiants[i].note_systeme);
    }
    return 0;
}