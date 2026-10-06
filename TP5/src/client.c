/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define _POSIX_C_SOURCE 200809L
#include "client.h"

#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int envoyer_tout(int socketfd, const char *donnees, size_t longueur) {
    size_t envoyes = 0;
    while (envoyes < longueur) {
        ssize_t resultat = send(socketfd, donnees + envoyes,
                                longueur - envoyes, 0);
        if (resultat < 0 && errno == EINTR) {
            continue;
        }
        if (resultat <= 0) {
            return -1;
        }
        envoyes += (size_t)resultat;
    }
    return 0;
}

static int echanger(int socketfd, const char *message, char *reponse,
                    size_t capacite) {
    size_t longueur = strlen(message);
    if (envoyer_tout(socketfd, message, longueur) != 0 ||
        envoyer_tout(socketfd, "\n", 1) != 0) {
        perror("Envoi au serveur");
        return -1;
    }

    size_t lus = 0;
    while (lus + 1 < capacite) {
        ssize_t resultat = recv(socketfd, reponse + lus, 1, 0);
        if (resultat < 0 && errno == EINTR) {
            continue;
        }
        if (resultat <= 0) {
            fprintf(stderr, "Le serveur a fermé la connexion.\n");
            return -1;
        }
        if (reponse[lus++] == '\n') {
            break;
        }
    }
    reponse[lus] = '\0';
    printf("%s\n", reponse);
    return 0;
}

int envoie_recois_message(int socketfd) {
    char message[1024];
    printf("Votre commande (message: ..., calcule : op a b, quit) : ");
    if (fgets(message, sizeof message, stdin) == NULL) {
        return -1;
    }
    message[strcspn(message, "\n")] = '\0';
    char reponse[TAILLE_MESSAGE];
    return echanger(socketfd, message, reponse, sizeof reponse);
}

static int lire_note(const char *chemin, long long *note) {
    FILE *fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        return -1;
    }
    char contenu[64];
    int resultat = -1;
    if (fgets(contenu, sizeof contenu, fichier) != NULL) {
        char *fin;
        errno = 0;
        long long valeur = strtoll(contenu, &fin, 10);
        while (*fin == ' ' || *fin == '\t' || *fin == '\r' || *fin == '\n') {
            ++fin;
        }
        if (errno == 0 && fin != contenu && *fin == '\0') {
            *note = valeur;
            resultat = 0;
        }
    }
    if (fclose(fichier) != 0) {
        return -1;
    }
    return resultat;
}

static int envoyer_calcul(int socketfd, char operateur,
                          long long gauche, long long droite,
                          long long *resultat) {
    char message[128];
    char reponse[TAILLE_MESSAGE];
    int longueur = snprintf(message, sizeof message, "calcule : %c %lld %lld",
                            operateur, gauche, droite);
    if (longueur < 0 || (size_t)longueur >= sizeof message ||
        echanger(socketfd, message, reponse, sizeof reponse) != 0) {
        return -1;
    }
    char *separateur = strchr(reponse, ':');
    if (separateur == NULL) {
        fprintf(stderr, "Réponse de calcul invalide : %s\n", reponse);
        return -1;
    }
    char *debut = separateur + 1;
    char *fin;
    errno = 0;
    long long valeur = strtoll(debut, &fin, 10);
    if (errno != 0 || fin == debut || (*fin != '\0' && *fin != '\n')) {
        fprintf(stderr, "Réponse de calcul invalide : %s\n", reponse);
        return -1;
    }
    *resultat = valeur;
    return 0;
}

static int traiter_notes(int socketfd, const char *repertoire) {
    long long somme_classe = 0;
    unsigned int nombre_notes = 0;
    for (unsigned int etudiant = 1; etudiant <= 5; ++etudiant) {
        long long somme_etudiant = 0;
        for (unsigned int indice = 1; indice <= 5; ++indice) {
            char chemin[PATH_MAX];
            int longueur = snprintf(chemin, sizeof chemin,
                                    "%s/%u/note%u.txt", repertoire,
                                    etudiant, indice);
            if (longueur < 0 || (size_t)longueur >= sizeof chemin) {
                fprintf(stderr, "Chemin de note trop long.\n");
                return -1;
            }
            long long note;
            if (lire_note(chemin, &note) != 0) {
                perror(chemin);
                return -1;
            }
            long long nouvelle_somme;
            if (nombre_notes == 0) {
                nouvelle_somme = note;
            } else if (envoyer_calcul(socketfd, '+', somme_classe, note,
                                      &nouvelle_somme) != 0) {
                return -1;
            }
            somme_classe = nouvelle_somme;
            if (indice == 1) {
                somme_etudiant = note;
            } else {
                if (envoyer_calcul(socketfd, '+', somme_etudiant, note,
                                   &somme_etudiant) != 0) {
                    return -1;
                }
            }
            ++nombre_notes;
        }
        printf("Somme des notes de l'étudiant %u calculée côté serveur.\n",
               etudiant);
    }
    long long moyenne;
    if (envoyer_calcul(socketfd, '/', somme_classe, nombre_notes,
                       &moyenne) != 0) {
        return -1;
    }
    printf("Total des %u notes : %lld; moyenne entière : %lld\n",
           nombre_notes, somme_classe, somme_classe / nombre_notes);
    return 0;
}

int main(int argc, char **argv) {
    struct sockaddr_in adresse_serveur;
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
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

    int resultat = EXIT_SUCCESS;
    if (argc == 2 && strcmp(argv[1], "notes") == 0) {
        if (traiter_notes(socketfd, "../etudiant") != 0) {
            fprintf(stderr, "Le traitement des notes a échoué.\n");
            resultat = EXIT_FAILURE;
        }
    } else if (argc == 2 && strncmp(argv[1], "notes=", 6) == 0) {
        if (traiter_notes(socketfd, argv[1] + 6) != 0) {
            fprintf(stderr, "Le traitement des notes a échoué.\n");
            resultat = EXIT_FAILURE;
        }
    } else if (argc != 1) {
        fprintf(stderr, "Usage : %s [notes|notes=<répertoire>]\n", argv[0]);
        resultat = EXIT_FAILURE;
    } else {
        char commande[1024];
        while (fgets(commande, sizeof commande, stdin) != NULL) {
            commande[strcspn(commande, "\n")] = '\0';
            if (strcmp(commande, "quit") == 0) {
                break;
            }
            char reponse[TAILLE_MESSAGE];
            if (echanger(socketfd, commande, reponse, sizeof reponse) != 0) {
                resultat = EXIT_FAILURE;
                break;
            }
        }
    }
    (void)shutdown(socketfd, SHUT_WR);
    close(socketfd);
    return resultat;
}
