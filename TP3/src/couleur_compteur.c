#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE 100
#define NOMBRE_COULEURS 8

struct Couleur {
    unsigned char rouge;
    unsigned char vert;
    unsigned char bleu;
    unsigned char alpha;
};

struct CouleurComptee {
    struct Couleur couleur;
    size_t occurrences;
};

static int couleurs_egales(struct Couleur gauche, struct Couleur droite) {
    return gauche.rouge == droite.rouge &&
           gauche.vert == droite.vert &&
           gauche.bleu == droite.bleu &&
           gauche.alpha == droite.alpha;
}

int main(void) {
    const struct Couleur palette[NOMBRE_COULEURS] = {
        {0xff, 0x23, 0x23, 0x45},
        {0xff, 0x00, 0x23, 0x12},
        {0x00, 0x80, 0xff, 0xff},
        {0x11, 0x22, 0x33, 0xff},
        {0x44, 0x55, 0x66, 0xff},
        {0x77, 0x88, 0x99, 0xff},
        {0xaa, 0xbb, 0xcc, 0xff},
        {0xde, 0xad, 0xbe, 0xef}
    };
    struct Couleur couleurs[TAILLE];
    struct CouleurComptee distinctes[TAILLE];
    size_t nombre_distinctes = 0;

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < TAILLE; ++i) {
        couleurs[i] = palette[rand() % NOMBRE_COULEURS];
    }

    for (size_t i = 0; i < TAILLE; ++i) {
        size_t j = 0;
        while (j < nombre_distinctes &&
               !couleurs_egales(distinctes[j].couleur, couleurs[i])) {
            ++j;
        }
        if (j == nombre_distinctes) {
            distinctes[j].couleur = couleurs[i];
            distinctes[j].occurrences = 0;
            ++nombre_distinctes;
        }
        ++distinctes[j].occurrences;
    }

    for (size_t i = 0; i < nombre_distinctes; ++i) {
        const struct Couleur couleur = distinctes[i].couleur;
        printf("0x%02x 0x%02x 0x%02x 0x%02x : %zu\n",
               (unsigned int)couleur.rouge,
               (unsigned int)couleur.vert,
               (unsigned int)couleur.bleu,
               (unsigned int)couleur.alpha,
               distinctes[i].occurrences);
    }
    return 0;
}