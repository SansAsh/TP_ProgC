/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TP6_SERVEUR_H
#define TP6_SERVEUR_H

#define PORT 8089
#define TAILLE_MESSAGE 8192

extern const char *svg_file_path;

int recois_envoie_message(int socket_client, char *json);

#endif
