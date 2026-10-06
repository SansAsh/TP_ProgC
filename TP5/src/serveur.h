/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TP5_SERVEUR_H
#define TP5_SERVEUR_H

#define PORT 8089
#define TAILLE_MESSAGE 2048

int renvoie_message(int socket_client, const char *message);
int recois_envoie_message(int socket_client, char *message);

#endif
