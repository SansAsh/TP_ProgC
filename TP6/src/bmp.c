#define _POSIX_C_SOURCE 200809L
#include "bmp.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

couleur_compteur *analyse_bmp_image(const char *nom_de_fichier) {
  if (nom_de_fichier == NULL) {
    fprintf(stderr, "Nom de fichier BMP manquant.\n");
    return NULL;
  }
  FILE *fichier = fopen(nom_de_fichier, "rb");
  if (fichier == NULL) {
    perror(nom_de_fichier);
    return NULL;
  }

  bmp_header entete;
  bmp_info_header information;
  couleur_compteur *compteurs = NULL;
  if (fread(&entete, sizeof entete, 1, fichier) != 1 ||
      fread(&information, sizeof information, 1, fichier) != 1) {
    fprintf(stderr, "%s : en-tête BMP incomplet.\n", nom_de_fichier);
    goto fin;
  }
  if (entete.type != 0x4d42 || information.info_header_size < 40 ||
      information.planes != 1 || information.compression != 0 ||
      (information.compte_bit != 24 && information.compte_bit != 32)) {
    fprintf(stderr, "%s : BMP non compressé 24/32 bits requis.\n",
            nom_de_fichier);
    goto fin;
  }

  int32_t hauteur_signee;
  memcpy(&hauteur_signee, &information.hauteur, sizeof hauteur_signee);
  int64_t hauteur_large = hauteur_signee < 0
                              ? -(int64_t)hauteur_signee
                              : hauteur_signee;
  if (information.largeur == 0 || hauteur_large == 0 ||
      information.largeur > INT_MAX || hauteur_large > INT_MAX ||
      (uint64_t)information.largeur * (uint64_t)hauteur_large > INT_MAX) {
    fprintf(stderr, "%s : dimensions BMP invalides ou trop grandes.\n",
            nom_de_fichier);
    goto fin;
  }

  size_t largeur = information.largeur;
  size_t hauteur = (size_t)hauteur_large;
  size_t octets_pixel = information.compte_bit / 8;
  if (largeur > (SIZE_MAX - 3) / octets_pixel) {
    fprintf(stderr, "%s : ligne BMP trop large.\n", nom_de_fichier);
    goto fin;
  }
  size_t octets_ligne = ((largeur * octets_pixel + 3) / 4) * 4;
  if (octets_ligne > SIZE_MAX / hauteur) {
    fprintf(stderr, "%s : image BMP trop grande.\n", nom_de_fichier);
    goto fin;
  }
  size_t taille_pixels = octets_ligne * hauteur;
  if (fseek(fichier, 0, SEEK_END) != 0) {
    perror("Lecture de la taille BMP");
    goto fin;
  }
  long taille_fichier = ftell(fichier);
  if (taille_fichier < 0 ||
      (uint64_t)entete.offset > (uint64_t)taille_fichier ||
      taille_pixels > (size_t)taille_fichier - (size_t)entete.offset) {
    fprintf(stderr, "%s : données de pixels BMP incomplètes.\n",
            nom_de_fichier);
    goto fin;
  }
  if (entete.offset < sizeof entete + information.info_header_size ||
      fseek(fichier, (long)entete.offset, SEEK_SET) != 0) {
    fprintf(stderr, "%s : décalage de pixels BMP invalide.\n",
            nom_de_fichier);
    goto fin;
  }

  couleur pixels;
  pixels.compte_bit = information.compte_bit == 24 ? BITS24 : BITS32;
  pixels.size = (int)(largeur * hauteur);
  pixels.c.c24 = NULL;
  if (pixels.compte_bit == BITS24) {
    pixels.c.c24 = malloc((size_t)pixels.size * sizeof *pixels.c.c24);
  } else {
    pixels.c.c32 = malloc((size_t)pixels.size * sizeof *pixels.c.c32);
  }
  void *stockage = pixels.compte_bit == BITS24
                       ? (void *)pixels.c.c24 : (void *)pixels.c.c32;
  unsigned char *ligne = malloc(octets_ligne);
  if (stockage == NULL || ligne == NULL) {
    perror("Allocation de l'image BMP");
    free(stockage);
    free(ligne);
    goto fin;
  }

  int valide = 1;
  for (size_t y = 0; y < hauteur && valide; ++y) {
    if (fread(ligne, 1, octets_ligne, fichier) != octets_ligne) {
      fprintf(stderr, "%s : données de pixels BMP incomplètes.\n",
              nom_de_fichier);
      valide = 0;
      break;
    }
    size_t y_source = hauteur_signee < 0 ? y : hauteur - 1 - y;
    for (size_t x = 0; x < largeur; ++x) {
      size_t indice = y_source * largeur + x;
      const unsigned char *pixel = ligne + x * octets_pixel;
      if (pixels.compte_bit == BITS24) {
        pixels.c.c24[indice] = (couleur24){pixel[2], pixel[1], pixel[0]};
      } else {
        pixels.c.c32[indice] =
            (couleur32){pixel[2], pixel[1], pixel[0], pixel[3]};
      }
    }
  }
  free(ligne);
  if (valide) {
    compteurs = compte_couleur(&pixels, pixels.size);
    if (compteurs != NULL) {
      trier_couleur_compteur(compteurs);
    }
  }
  free(stockage);

fin:
  if (fclose(fichier) != 0) {
    perror(nom_de_fichier);
    libere_couleur_compteur(compteurs);
    return NULL;
  }
  return compteurs;
}
