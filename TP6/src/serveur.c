#define _POSIX_C_SOURCE 200809L
#include "serveur.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const char *svg_file_path = "pie_chart.svg";
extern char **environ;

static void ignorer_espaces(const char **position) {
  while (isspace((unsigned char)**position)) ++*position;
}

static int lire_json_chaine(const char **position, char *destination,
                            size_t capacite) {
  ignorer_espaces(position);
  if (**position != '"') return -1;
  ++*position;
  size_t taille = 0;
  while (**position != '\0' && **position != '"') {
    unsigned char caractere = (unsigned char)*(*position)++;
    if (caractere == '\\') {
      caractere = (unsigned char)*(*position)++;
      switch (caractere) {
      case '"': case '\\': case '/': break;
      case 'n': caractere = '\n'; break;
      case 'r': caractere = '\r'; break;
      case 't': caractere = '\t'; break;
      case 'b': caractere = '\b'; break;
      case 'f': caractere = '\f'; break;
      case 'u': {
        unsigned int code = 0;
        for (int i = 0; i < 4; ++i) {
          if (**position == '\0') return -1;
          unsigned char chiffre = (unsigned char)*(*position)++;
          if (!isxdigit(chiffre)) return -1;
          code = code * 16 + (unsigned int)(isdigit(chiffre)
                    ? chiffre - '0'
                    : tolower(chiffre) - 'a' + 10);
        }
        if (code == 0 || code > 0x7f) return -1;
        caractere = (unsigned char)code;
        break;
      }
      default: return -1;
      }
    }
    if (taille + 1 >= capacite || caractere == '\0') return -1;
    destination[taille++] = (char)caractere;
  }
  if (**position != '"') return -1;
  ++*position;
  destination[taille] = '\0';
  return 0;
}

static const char *trouver_valeur(const char *json, const char *cle) {
  char recherche[64];
  int longueur = snprintf(recherche, sizeof recherche, "\"%s\"", cle);
  if (longueur < 0 || (size_t)longueur >= sizeof recherche) return NULL;
  const char *position = strstr(json, recherche);
  if (position == NULL) return NULL;
  position += (size_t)longueur;
  ignorer_espaces(&position);
  if (*position++ != ':') return NULL;
  ignorer_espaces(&position);
  return position;
}

static int lire_champ_chaine(const char *json, const char *cle,
                             char *destination, size_t capacite) {
  const char *position = trouver_valeur(json, cle);
  return position == NULL ? -1
                          : lire_json_chaine(&position, destination, capacite);
}

static int lire_nombre(const char *json, const char *cle, unsigned int *nombre) {
  const char *position = trouver_valeur(json, cle);
  if (position == NULL || !isdigit((unsigned char)*position)) return -1;
  char *fin;
  errno = 0;
  unsigned long valeur = strtoul(position, &fin, 10);
  while (isspace((unsigned char)*fin)) ++fin;
  if (errno != 0 || (*fin != ',' && *fin != '}') || valeur > 30) return -1;
  *nombre = (unsigned int)valeur;
  return 0;
}

static int lire_valeurs(const char *json, char valeurs[30][TAILLE_MESSAGE],
                        size_t *nombre) {
  const char *position = trouver_valeur(json, "valeurs");
  if (position == NULL || *position++ != '[') return -1;
  *nombre = 0;
  ignorer_espaces(&position);
  if (*position == ']') return 0;
  for (;;) {
    if (*nombre >= 30 ||
        lire_json_chaine(&position, valeurs[*nombre], sizeof valeurs[0]) != 0) {
      return -1;
    }
    ++*nombre;
    ignorer_espaces(&position);
    if (*position == ']') return 0;
    if (*position++ != ',') return -1;
  }
}

static int envoyer_tout(int socket_client, const char *donnees,
                        size_t longueur) {
  size_t envoyes = 0;
  while (envoyes < longueur) {
    ssize_t resultat = send(socket_client, donnees + envoyes,
                            longueur - envoyes, 0);
    if (resultat < 0 && errno == EINTR) continue;
    if (resultat <= 0) return -1;
    envoyes += (size_t)resultat;
  }
  return 0;
}

static int ajouter_json_chaine(char *json, size_t capacite, size_t *taille,
                               const char *texte) {
  if (*taille + 1 >= capacite) return -1;
  json[(*taille)++] = '"';
  for (const unsigned char *caractere = (const unsigned char *)texte;
       *caractere != '\0'; ++caractere) {
    char echappe[7];
    const char *fragment = echappe;
    if (*caractere == '"' || *caractere == '\\') {
      echappe[0] = '\\';
      echappe[1] = (char)*caractere;
      echappe[2] = '\0';
    } else if (*caractere == '\n') fragment = "\\n";
    else if (*caractere == '\r') fragment = "\\r";
    else if (*caractere == '\t') fragment = "\\t";
    else if (*caractere < 0x20) {
      (void)snprintf(echappe, sizeof echappe, "\\u%04x", *caractere);
    } else {
      echappe[0] = (char)*caractere;
      echappe[1] = '\0';
    }
    size_t longueur = strlen(fragment);
    if (*taille + longueur + 1 >= capacite) return -1;
    memcpy(json + *taille, fragment, longueur);
    *taille += longueur;
  }
  json[(*taille)++] = '"';
  json[*taille] = '\0';
  return 0;
}

static int repondre_json(int socket_client, const char *code,
                         const char *message) {
  char reponse[TAILLE_MESSAGE];
  size_t taille = 0;
  const char *prefixe = "{\"code\":";
  memcpy(reponse, prefixe, strlen(prefixe));
  taille = strlen(prefixe);
  reponse[taille] = '\0';
  if (ajouter_json_chaine(reponse, sizeof reponse, &taille, code) != 0 ||
      taille + strlen(",\"valeurs\":[") >= sizeof reponse) return -1;
  memcpy(reponse + taille, ",\"valeurs\":[", strlen(",\"valeurs\":["));
  taille += strlen(",\"valeurs\":[");
  reponse[taille] = '\0';
  if (ajouter_json_chaine(reponse, sizeof reponse, &taille, message) != 0 ||
      taille + 2 >= sizeof reponse) return -1;
  memcpy(reponse + taille, "]}", 2);
  taille += 2;
  reponse[taille] = '\0';
  return envoyer_tout(socket_client, reponse, taille) == 0 &&
                 envoyer_tout(socket_client, "\n", 1) == 0
             ? 0 : -1;
}

static int couleur_valide(const char *couleur) {
  if (couleur[0] != '#' || strlen(couleur) != 7) return 0;
  for (size_t i = 1; i < 7; ++i) {
    if (!isxdigit((unsigned char)couleur[i])) return 0;
  }
  return 1;
}

static int creer_graphique(char couleurs[30][TAILLE_MESSAGE], size_t nombre) {
  FILE *fichier = fopen(svg_file_path, "w");
  if (fichier == NULL) {
    perror(svg_file_path);
    return -1;
  }
  fprintf(fichier, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fichier, "<svg xmlns=\"http://www.w3.org/2000/svg\" "
                   "width=\"400\" height=\"400\" viewBox=\"0 0 400 400\">\n");
  fprintf(fichier, "<rect width=\"400\" height=\"400\" fill=\"white\"/>\n");
  const double centre_x = 200.0;
  const double centre_y = 200.0;
  const double rayon = 150.0;
  if (nombre == 1) {
    fprintf(fichier, "<circle cx=\"%.1f\" cy=\"%.1f\" r=\"%.1f\" "
                     "fill=\"%s\"/>\n",
            centre_x, centre_y, rayon, couleurs[0]);
  } else {
    double angle_depart = -90.0;
    for (size_t i = 0; i < nombre; ++i) {
      double angle_fin = angle_depart + 360.0 / (double)nombre;
      double debut_rad = angle_depart * acos(-1.0) / 180.0;
      double fin_rad = angle_fin * acos(-1.0) / 180.0;
      double x1 = centre_x + rayon * cos(debut_rad);
      double y1 = centre_y + rayon * sin(debut_rad);
      double x2 = centre_x + rayon * cos(fin_rad);
      double y2 = centre_y + rayon * sin(fin_rad);
      int grand_arc = angle_fin - angle_depart > 180.0;
      fprintf(fichier,
              "<path d=\"M %.2f %.2f A %.2f %.2f 0 %d 1 %.2f %.2f "
              "L %.2f %.2f Z\" fill=\"%s\"/>\n",
              x1, y1, rayon, rayon, grand_arc, x2, y2, centre_x, centre_y,
              couleurs[i]);
      angle_depart = angle_fin;
    }
  }
  int erreur = fputs("</svg>\n", fichier) == EOF;
  if (fclose(fichier) != 0) erreur = 1;
  if (erreur) {
    perror("Écriture du graphique SVG");
    return -1;
  }

  if (getenv("DISPLAY") != NULL) {
    char *arguments[] = {(char *)"firefox", (char *)svg_file_path, NULL};
    pid_t navigateur;
    int resultat = posix_spawnp(&navigateur, "firefox", NULL, NULL,
                                arguments, environ);
    if (resultat != 0) {
      fprintf(stderr, "Graphique créé; Firefox indisponible (%s).\n",
              strerror(resultat));
    }
  }
  return 0;
}

int recois_envoie_message(int socket_client, char *json) {
  char code[32];
  if (lire_champ_chaine(json, "code", code, sizeof code) != 0) {
    return repondre_json(socket_client, "erreur", "JSON invalide : code absent");
  }
  char valeurs[30][TAILLE_MESSAGE];
  size_t nombre_valeurs;
  if (lire_valeurs(json, valeurs, &nombre_valeurs) != 0) {
    return repondre_json(socket_client, "erreur",
                         "JSON invalide : valeurs attendues");
  }
  if (strcmp(code, "message") == 0) {
    if (nombre_valeurs != 1) {
      return repondre_json(socket_client, "erreur",
                           "Un message doit contenir une valeur.");
    }
    return repondre_json(socket_client, "message", valeurs[0]);
  }
  if (strcmp(code, "couleurs") == 0) {
    unsigned int nombre;
    if (lire_nombre(json, "nombre", &nombre) != 0 ||
        nombre == 0 || nombre > 30 || nombre != nombre_valeurs) {
      return repondre_json(socket_client, "erreur",
                           "Le nombre de couleurs doit être entre 1 et 30.");
    }
    for (size_t i = 0; i < nombre_valeurs; ++i) {
      if (!couleur_valide(valeurs[i])) {
        return repondre_json(socket_client, "erreur",
                             "Format de couleur invalide.");
      }
    }
    if (creer_graphique(valeurs, nombre_valeurs) != 0) {
      return repondre_json(socket_client, "erreur",
                           "Impossible de créer le graphique SVG.");
    }
    return repondre_json(socket_client, "resultat", svg_file_path);
  }
  return repondre_json(socket_client, "erreur", "Code d'opération inconnu.");
}

static int lire_ligne(int socket_client, char *buffer, size_t capacite) {
  size_t longueur = 0;
  while (longueur + 1 < capacite) {
    char caractere;
    ssize_t resultat = recv(socket_client, &caractere, 1, 0);
    if (resultat < 0 && errno == EINTR) continue;
    if (resultat == 0) return longueur == 0 ? 0 : (int)longueur;
    if (resultat < 0) return -1;
    if (caractere == '\n') {
      buffer[longueur] = '\0';
      return (int)longueur;
    }
    if (caractere != '\r') buffer[longueur++] = caractere;
  }
  buffer[longueur] = '\0';
  return -2;
}

int main(void) {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }
  int option = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option,
                 sizeof option) != 0) {
    perror("setsockopt");
    close(socketfd);
    return EXIT_FAILURE;
  }
  struct sockaddr_in adresse;
  memset(&adresse, 0, sizeof adresse);
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(PORT);
  adresse.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(socketfd, (struct sockaddr *)&adresse, sizeof adresse) != 0) {
    perror("bind");
    close(socketfd);
    return EXIT_FAILURE;
  }
  if (listen(socketfd, 10) != 0) {
    perror("listen");
    close(socketfd);
    return EXIT_FAILURE;
  }
  printf("Serveur en attente de connexions sur le port %d...\n", PORT);

  for (;;) {
    int socket_client = accept(socketfd, NULL, NULL);
    if (socket_client < 0) {
      if (errno == EINTR) continue;
      perror("accept");
      close(socketfd);
      return EXIT_FAILURE;
    }
    pid_t enfant = fork();
    if (enfant < 0) {
      perror("fork");
      close(socket_client);
      continue;
    }
    if (enfant == 0) {
      close(socketfd);
      char json[TAILLE_MESSAGE];
      int longueur = lire_ligne(socket_client, json, sizeof json);
      if (longueur > 0) {
        if (recois_envoie_message(socket_client, json) != 0) {
          perror("Réponse au client");
        }
      } else {
        (void)repondre_json(socket_client, "erreur",
                            "Requête absente ou trop longue.");
      }
      close(socket_client);
      _exit(EXIT_SUCCESS);
    }
    close(socket_client);
  }
}
