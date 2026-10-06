/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TP5_CLIENT_H
#define TP5_CLIENT_H

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089
#define TAILLE_MESSAGE 2048

int envoie_recois_message(int socketfd);

#endif
