#include <stdio.h>

#define LONGUEUR_PHRASE 160

int main(void) {
    const char *phrases[10] = {
        "Bonjour, comment ça va ?",
        "Le temps est magnifique aujourd'hui.",
        "C'est une belle journée.",
        "La programmation en C est amusante.",
        "Les tableaux en C sont puissants.",
        "Les pointeurs en C peuvent être déroutants.",
        "Il fait beau dehors.",
        "La recherche dans un tableau est intéressante.",
        "Les structures de données sont importantes.",
        "Programmer en C, c'est génial."
    };
    char recherche[LONGUEUR_PHRASE];

    printf("Entrez la phrase a rechercher : ");
    if (fgets(recherche, sizeof recherche, stdin) == NULL) {
        fprintf(stderr, "Impossible de lire la phrase.\n");
        return 1;
    }

    size_t longueur = 0;
    while (recherche[longueur] != '\0' && recherche[longueur] != '\n') {
        ++longueur;
    }
    if (recherche[longueur] == '\n') {
        recherche[longueur] = '\0';
    }

    int trouvee = 0;
    for (size_t i = 0; i < sizeof phrases / sizeof phrases[0] && !trouvee;
         ++i) {
        size_t j = 0;
        while (recherche[j] != '\0' &&
               phrases[i][j] != '\0' &&
               recherche[j] == phrases[i][j]) {
            ++j;
        }
        trouvee = recherche[j] == '\0' && phrases[i][j] == '\0';
    }

    printf("%s\n", trouvee ? "Phrase trouvée" : "Phrase non trouvée");
    return 0;
}
