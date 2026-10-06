#define _POSIX_C_SOURCE 200809L
#include "serveur.h"

#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int envoyer_tout(int socket_client, const char *message,
                        size_t longueur) {
    size_t envoyes = 0;
    while (envoyes < longueur) {
        ssize_t resultat = send(socket_client, message + envoyes,
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

int renvoie_message(int socket_client, const char *message) {
    return envoyer_tout(socket_client, message, strlen(message));
}

static int calculer(char operateur, long long gauche, long long droite,
                    long long *resultat) {
    switch (operateur) {
    case '+':
        if (__builtin_add_overflow(gauche, droite, resultat)) return -1;
        return 0;
    case '-':
        if (__builtin_sub_overflow(gauche, droite, resultat)) return -1;
        return 0;
    case '*':
        if (__builtin_mul_overflow(gauche, droite, resultat)) return -1;
        return 0;
    case '/':
        if (droite == 0 || (gauche == LLONG_MIN && droite == -1)) return -1;
        *resultat = gauche / droite;
        return 0;
    case '%':
        if (droite == 0 || (gauche == LLONG_MIN && droite == -1)) return -1;
        *resultat = gauche % droite;
        return 0;
    case '&': *resultat = gauche & droite; return 0;
    case '|': *resultat = gauche | droite; return 0;
    case '~': *resultat = ~gauche; return 0;
    default: return -1;
    }
}

static int traiter_calcul(int socket_client, const char *message) {
    char operateur;
    long long gauche;
    long long droite = 0;
    char supplement;
    int nombre_champs = sscanf(message, "calcule : %c %lld %lld %c",
                               &operateur, &gauche, &droite, &supplement);
    if (nombre_champs < 2 || nombre_champs > 3) {
        return renvoie_message(socket_client, "erreur : commande de calcul invalide");
    }
    if (operateur != '~' && nombre_champs != 3) {
        return renvoie_message(socket_client, "erreur : deux opérandes requises");
    }
    if (operateur == '~' && nombre_champs == 2) {
        droite = 0;
    } else if (nombre_champs == 3) {
        char *fin;
        (void)fin;
    }
    long long resultat;
    if (calculer(operateur, gauche, droite, &resultat) != 0) {
        return renvoie_message(socket_client, "erreur : opération invalide");
    }
    char reponse[128];
    int longueur = snprintf(reponse, sizeof reponse, "calcule : %lld",
                            resultat);
    if (longueur < 0 || (size_t)longueur >= sizeof reponse) {
        return -1;
    }
    return renvoie_message(socket_client, reponse);
}

int recois_envoie_message(int socket_client, char *message) {
    printf("Message reçu : %s\n", message);
    if (strncmp(message, "message:", 8) == 0) {
        return renvoie_message(socket_client, message);
    }
    if (strncmp(message, "calcule :", 9) == 0) {
        return traiter_calcul(socket_client, message);
    }
    return renvoie_message(socket_client, "erreur : commande inconnue");
}

static int lire_ligne(int socket_client, char *buffer, size_t capacite) {
    size_t longueur = 0;
    while (longueur + 1 < capacite) {
        char caractere;
        ssize_t resultat = recv(socket_client, &caractere, 1, 0);
        if (resultat < 0 && errno == EINTR) {
            continue;
        }
        if (resultat == 0) {
            return longueur == 0 ? 0 : (int)longueur;
        }
        if (resultat < 0) {
            return -1;
        }
        if (caractere == '\n') {
            buffer[longueur] = '\0';
            return (int)longueur;
        }
        if (caractere != '\r') {
            buffer[longueur++] = caractere;
        }
    }
    buffer[longueur] = '\0';
    return -2;
}

static void gerer_client(int socket_client) {
    char message[TAILLE_MESSAGE];
    for (;;) {
        int longueur = lire_ligne(socket_client, message, sizeof message);
        if (longueur == 0) {
            break;
        }
        if (longueur < 0) {
            if (longueur == -1) perror("Lecture client");
            else (void)renvoie_message(socket_client, "erreur : message trop long");
            break;
        }
        if (recois_envoie_message(socket_client, message) != 0 ||
            envoyer_tout(socket_client, "\n", 1) != 0) {
            perror("Réponse client");
            break;
        }
    }
    close(socket_client);
}

static void reap_child(int signal_num) {
    (void)signal_num;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);
    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = reap_child;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &action, NULL) != 0) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

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
            gerer_client(socket_client);
            _exit(EXIT_SUCCESS);
        }
        close(socket_client);
    }
}
