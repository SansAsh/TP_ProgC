#include "couleur.h"

#include <stdio.h>
#include <stdlib.h>

static uint32_t cle_couleur(const couleur *couleurs, int indice) {
  if (couleurs->compte_bit == BITS24) {
    const couleur24 pixel = couleurs->c.c24[indice];
    return ((uint32_t)pixel.rouge << 16) |
           ((uint32_t)pixel.vert << 8) | pixel.bleu;
  }
  const couleur32 pixel = couleurs->c.c32[indice];
  return ((uint32_t)pixel.rouge << 24) |
         ((uint32_t)pixel.vert << 16) |
         ((uint32_t)pixel.bleu << 8) | pixel.alpha;
}

static int comparer_compteurs(const void *gauche, const void *droite) {
  const couleur24_compteur *a24 = gauche;
  const couleur24_compteur *b24 = droite;
  return a24->compte < b24->compte ? 1 : a24->compte > b24->compte ? -1 : 0;
}

static int comparer_compteurs32(const void *gauche, const void *droite) {
  const couleur32_compteur *a = gauche;
  const couleur32_compteur *b = droite;
  return a->compte < b->compte ? 1 : a->compte > b->compte ? -1 : 0;
}

couleur_compteur *compte_couleur(couleur *couleurs, int taille) {
  if (couleurs == NULL || taille < 0 ||
      (couleurs->compte_bit != BITS24 && couleurs->compte_bit != BITS32)) {
    fprintf(stderr, "Données de couleurs invalides.\n");
    return NULL;
  }

  couleur_compteur *compteurs = calloc(1, sizeof *compteurs);
  if (compteurs == NULL) {
    perror("Allocation du compteur de couleurs");
    return NULL;
  }
  compteurs->compte_bit = couleurs->compte_bit;
  compteurs->size = 0;
  if (taille == 0) {
    return compteurs;
  }

  if ((size_t)taille > SIZE_MAX / 2) {
    free(compteurs);
    fprintf(stderr, "Image trop grande pour compter les couleurs.\n");
    return NULL;
  }
  size_t capacite = 1;
  while (capacite < (size_t)taille * 2) {
    if (capacite > SIZE_MAX / 2) {
      free(compteurs);
      fprintf(stderr, "Image trop grande pour compter les couleurs.\n");
      return NULL;
    }
    capacite *= 2;
  }
  size_t *table = calloc(capacite, sizeof *table);
  uint32_t *cles = malloc((size_t)taille * sizeof *cles);
  if (table == NULL || cles == NULL) {
    perror("Allocation de la table de couleurs");
    free(cles);
    free(table);
    free(compteurs);
    return NULL;
  }
  if (couleurs->compte_bit == BITS24) {
    compteurs->cc.cc24 = calloc((size_t)taille, sizeof *compteurs->cc.cc24);
    if (compteurs->cc.cc24 == NULL) {
      perror("Allocation des couleurs distinctes");
      free(cles);
      free(table);
      free(compteurs);
      return NULL;
    }
  } else {
    compteurs->cc.cc32 = calloc((size_t)taille, sizeof *compteurs->cc.cc32);
    if (compteurs->cc.cc32 == NULL) {
      perror("Allocation des couleurs distinctes");
      free(cles);
      free(table);
      free(compteurs);
      return NULL;
    }
  }

  for (int i = 0; i < taille; ++i) {
    uint32_t cle = cle_couleur(couleurs, i);
    size_t position = ((uint64_t)cle * UINT64_C(11400714819323198485)) &
                      (capacite - 1);
    while (table[position] != 0 &&
           cles[table[position] - 1] != cle) {
      position = (position + 1) & (capacite - 1);
    }
    if (table[position] == 0) {
      size_t indice = (size_t)compteurs->size++;
      table[position] = indice + 1;
      cles[indice] = cle;
      if (couleurs->compte_bit == BITS24) {
        compteurs->cc.cc24[indice].c = couleurs->c.c24[i];
        compteurs->cc.cc24[indice].compte = 1;
      } else {
        compteurs->cc.cc32[indice].c = couleurs->c.c32[i];
        compteurs->cc.cc32[indice].compte = 1;
      }
    } else if (couleurs->compte_bit == BITS24) {
      ++compteurs->cc.cc24[table[position] - 1].compte;
    } else {
      ++compteurs->cc.cc32[table[position] - 1].compte;
    }
  }
  free(cles);
  free(table);
  return compteurs;
}

void print_couleur(couleur *couleurs, int taille) {
  if (couleurs == NULL) return;
  for (int i = 0; i < taille; ++i) {
    if (couleurs->compte_bit == BITS24) {
      printf("%02x %02x %02x\n", couleurs->c.c24[i].rouge,
             couleurs->c.c24[i].vert, couleurs->c.c24[i].bleu);
    } else if (couleurs->compte_bit == BITS32) {
      printf("%02x %02x %02x %02x\n", couleurs->c.c32[i].rouge,
             couleurs->c.c32[i].vert, couleurs->c.c32[i].bleu,
             couleurs->c.c32[i].alpha);
    }
  }
}

void print_couleur_compteur(couleur_compteur *compteurs) {
  if (compteurs == NULL) return;
  for (int i = 0; i < compteurs->size; ++i) {
    if (compteurs->compte_bit == BITS24) {
      const couleur24_compteur pixel = compteurs->cc.cc24[i];
      printf("#%02x%02x%02x: %d\n", pixel.c.rouge, pixel.c.vert,
             pixel.c.bleu, pixel.compte);
    } else if (compteurs->compte_bit == BITS32) {
      const couleur32_compteur pixel = compteurs->cc.cc32[i];
      printf("#%02x%02x%02x%02x: %d\n", pixel.c.rouge, pixel.c.vert,
             pixel.c.bleu, pixel.c.alpha, pixel.compte);
    }
  }
}

void trier_couleur_compteur(couleur_compteur *compteurs) {
  if (compteurs == NULL || compteurs->size < 2) return;
  if (compteurs->compte_bit == BITS24) {
    qsort(compteurs->cc.cc24, (size_t)compteurs->size,
          sizeof *compteurs->cc.cc24, comparer_compteurs);
  } else if (compteurs->compte_bit == BITS32) {
    qsort(compteurs->cc.cc32, (size_t)compteurs->size,
          sizeof *compteurs->cc.cc32, comparer_compteurs32);
  }
}

void libere_couleur_compteur(couleur_compteur *compteurs) {
  if (compteurs == NULL) return;
  if (compteurs->compte_bit == BITS24) {
    free(compteurs->cc.cc24);
  } else if (compteurs->compte_bit == BITS32) {
    free(compteurs->cc.cc32);
  }
  free(compteurs);
}
