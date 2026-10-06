#define _POSIX_C_SOURCE 200809L
#include "client.h"

#include "bmp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TAILLE_JSON 8192

static int ajouter_texte(char *destination, size_t capacite, size_t *taille,
                         const char *texte) {
  size_t longueur = strlen(texte);
  if (*taille + longueur >= capacite) return -1;
  memcpy(destination + *taille, texte, longueur);
  *taille += longueur;
  destination[*taille] = '\0';
  return 0;
}

static int ajouter_json_chaine(char *destination, size_t capacite,
                               size_t *taille, const char *texte) {
  if (ajouter_texte(destination, capacite, taille, "\"") != 0) return -1;
  for (const unsigned char *caractere = (const unsigned char *)texte;
       *caractere != '\0'; ++caractere) {
    char echappe[7];
    const char *fragment = echappe;
    if (*caractere == '"' || *caractere == '\\') {
      echappe[0] = '\\';
      echappe[1] = (char)*caractere;
      echappe[2] = '\0';
    } else if (*caractere == '\n') {
      fragment = "\\n";
    } else if (*caractere == '\r') {
      fragment = "\\r";
    } else if (*caractere == '\t') {
      fragment = "\\t";
    } else if (*caractere < 0x20) {
      (void)snprintf(echappe, sizeof echappe, "\\u%04x", *caractere);
    } else {
      echappe[0] = (char)*caractere;
      echappe[1] = '\0';
    }
    if (ajouter_texte(destination, capacite, taille, fragment) != 0) return -1;
  }
  return ajouter_texte(destination, capacite, taille, "\"");
}

static int envoyer_tout(int socketfd, const char *donnees, size_t longueur) {
  size_t envoyes = 0;
  while (envoyes < longueur) {
    ssize_t resultat = send(socketfd, donnees + envoyes, longueur - envoyes, 0);
    if (resultat < 0 && errno == EINTR) continue;
    if (resultat <= 0) return -1;
    envoyes += (size_t)resultat;
  }
  return 0;
}

static int envoyer_recevoir(int socketfd, const char *json) {
  if (envoyer_tout(socketfd, json, strlen(json)) != 0 ||
      envoyer_tout(socketfd, "\n", 1) != 0) {
    perror("Envoi au serveur");
    return -1;
  }
  char reponse[TAILLE_JSON];
  size_t longueur = 0;
  while (longueur + 1 < sizeof reponse) {
    ssize_t resultat = recv(socketfd, reponse + longueur, 1, 0);
    if (resultat < 0 && errno == EINTR) continue;
    if (resultat <= 0) {
      fprintf(stderr, "Le serveur a fermé la connexion.\n");
      return -1;
    }
    if (reponse[longueur++] == '\n') break;
  }
  reponse[longueur] = '\0';
  printf("Réponse du serveur : %s\n", reponse);
  return 0;
}

static int creer_message_json(const char *message, char *json,
                              size_t capacite) {
  size_t taille = 0;
  json[0] = '\0';
  if (ajouter_texte(json, capacite, &taille,
                    "{\"code\":\"message\",\"valeurs\":[") != 0 ||
      ajouter_json_chaine(json, capacite, &taille, message) != 0 ||
      ajouter_texte(json, capacite, &taille, "]}") != 0) {
    fprintf(stderr, "Message trop long pour le format JSON.\n");
    return -1;
  }
  return 0;
}

static int compter_couleurs(const char *chemin, unsigned int demande,
                            char *json, size_t capacite) {
  couleur_compteur *compteurs = analyse_bmp_image(chemin);
  if (compteurs == NULL) return -1;
  unsigned int nombre = (unsigned int)compteurs->size;
  if (nombre > demande) nombre = demande;
  if (nombre == 0) {
    libere_couleur_compteur(compteurs);
    fprintf(stderr, "L'image ne contient aucune couleur exploitable.\n");
    return -1;
  }

  size_t taille = 0;
  char champ_nombre[64];
  int longueur = snprintf(champ_nombre, sizeof champ_nombre,
                          "{\"code\":\"couleurs\",\"nombre\":%u,"
                          "\"valeurs\":[", nombre);
  if (longueur < 0 || (size_t)longueur >= sizeof champ_nombre ||
      ajouter_texte(json, capacite, &taille, champ_nombre) != 0) {
    libere_couleur_compteur(compteurs);
    return -1;
  }
  for (unsigned int i = 0; i < nombre; ++i) {
    char couleur[16];
    if (compteurs->compte_bit == BITS24) {
      const couleur24 pixel = compteurs->cc.cc24[i].c;
      (void)snprintf(couleur, sizeof couleur, "#%02x%02x%02x",
                     pixel.rouge, pixel.vert, pixel.bleu);
    } else {
      const couleur32 pixel = compteurs->cc.cc32[i].c;
      (void)snprintf(couleur, sizeof couleur, "#%02x%02x%02x",
                     pixel.rouge, pixel.vert, pixel.bleu);
    }
    if ((i > 0 && ajouter_texte(json, capacite, &taille, ",") != 0) ||
        ajouter_json_chaine(json, capacite, &taille, couleur) != 0) {
      libere_couleur_compteur(compteurs);
      fprintf(stderr, "Liste de couleurs trop longue pour le JSON.\n");
      return -1;
    }
  }
  libere_couleur_compteur(compteurs);
  if (ajouter_texte(json, capacite, &taille, "]}") != 0) return -1;
  return 0;
}

static int convertir_nombre(const char *texte, unsigned int *nombre) {
  char *fin;
  errno = 0;
  unsigned long valeur = strtoul(texte, &fin, 10);
  if (errno != 0 || *texte == '\0' || *fin != '\0' ||
      valeur < 1 || valeur > 30) {
    return -1;
  }
  *nombre = (unsigned int)valeur;
  return 0;
}

static int demander_nombre(unsigned int *nombre) {
  char entree[32];
  printf("Nombre de couleurs à extraire (1-30) : ");
  if (fgets(entree, sizeof entree, stdin) == NULL) return -1;
  entree[strcspn(entree, "\n")] = '\0';
  return convertir_nombre(entree, nombre);
}

int main(int argc, char **argv) {
  char json[TAILLE_JSON];
  int socketfd;
  struct sockaddr_in adresse_serveur;
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage : %s <image.bmp> [nombre_de_couleurs]\n"
                    "       %s --message <texte>\n", argv[0], argv[0]);
    return EXIT_FAILURE;
  }

  int est_message = strcmp(argv[1], "--message") == 0;
  if (est_message) {
    if (argc != 3 || creer_message_json(argv[2], json, sizeof json) != 0) {
      fprintf(stderr, "Usage : %s --message <texte>\n", argv[0]);
      return EXIT_FAILURE;
    }
  } else {
    unsigned int nombre;
    if (argc == 3) {
      if (convertir_nombre(argv[2], &nombre) != 0) {
        fprintf(stderr, "Le nombre de couleurs doit être compris entre 1 et 30.\n");
        return EXIT_FAILURE;
      }
    } else if (demander_nombre(&nombre) != 0) {
      fprintf(stderr, "Nombre de couleurs invalide.\n");
      return EXIT_FAILURE;
    }
    if (compter_couleurs(argv[1], nombre, json, sizeof json) != 0) {
      return EXIT_FAILURE;
    }
  }

  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }
  memset(&adresse_serveur, 0, sizeof adresse_serveur);
  adresse_serveur.sin_family = AF_INET;
  adresse_serveur.sin_port = htons(PORT);
  adresse_serveur.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(socketfd, (struct sockaddr *)&adresse_serveur,
              sizeof adresse_serveur) != 0) {
    perror("Connexion au serveur");
    close(socketfd);
    return EXIT_FAILURE;
  }
  int resultat = envoyer_recevoir(socketfd, json);
  shutdown(socketfd, SHUT_WR);
  close(socketfd);
  return resultat == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
