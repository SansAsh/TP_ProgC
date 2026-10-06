#include <stdio.h>
#include <string.h>

#define TAILLE_LIGNE 4096
#define TAILLE_CHEMIN 1024
#define TAILLE_PHRASE 1024

int main(int argc, char **argv) {
    char chemin[TAILLE_CHEMIN];
    char phrase[TAILLE_PHRASE];
    if (argc > 2) {
        fprintf(stderr, "Usage : %s [fichier]\n", argv[0]);
        return 1;
    }
    if (argc == 2) {
        if (strlen(argv[1]) >= sizeof chemin) {
            fprintf(stderr, "Chemin trop long.\n");
            return 1;
        }
        strcpy(chemin, argv[1]);
    } else {
        printf("Entrez le nom du fichier : ");
        if (fgets(chemin, sizeof chemin, stdin) == NULL) {
            fprintf(stderr, "Impossible de lire le nom de fichier.\n");
            return 1;
        }
        chemin[strcspn(chemin, "\n")] = '\0';
    }
    printf("Entrez la phrase que vous souhaitez rechercher : ");
    if (fgets(phrase, sizeof phrase, stdin) == NULL) {
        fprintf(stderr, "Impossible de lire la phrase.\n");
        return 1;
    }
    phrase[strcspn(phrase, "\n")] = '\0';
    if (phrase[0] == '\0') {
        fprintf(stderr, "La phrase à rechercher ne peut pas être vide.\n");
        return 1;
    }

    FILE *fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        perror(chemin);
        return 1;
    }
    printf("Résultats de la recherche :\n");
    char ligne[TAILLE_LIGNE];
    size_t numero_ligne = 0;
    while (fgets(ligne, sizeof ligne, fichier) != NULL) {
        ++numero_ligne;
        size_t occurrences = 0;
        const char *position = ligne;
        while ((position = strstr(position, phrase)) != NULL) {
            ++occurrences;
            ++position;
        }
        if (occurrences > 0) {
            printf("Ligne %zu, %zu fois\n", numero_ligne, occurrences);
        }
    }
    if (ferror(fichier)) {
        perror(chemin);
        fclose(fichier);
        return 1;
    }
    if (fclose(fichier) != 0) {
        perror(chemin);
        return 1;
    }
    return 0;
}