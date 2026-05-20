#include <glib.h>
#include <gio/gio.h>
#include "netutils/communication.h"

#ifndef GAME_MANAGER
#define GAME_MANAGER

void start_game(GSocketConnection *player1, GSocketConnection *player2);

#endif
