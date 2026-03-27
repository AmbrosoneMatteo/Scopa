#include <glib.h>
#include <gio/gio.h>

#include "game-manager.h"

struct GameContext {
  GSocketConnection *player1;
  GSocketConnection *player2;
};
struct GameContext game = {NULL, NULL};

void start_game(GSocketConnection *player1, GSocketConnection *player2){
  game.player1 = player1;
  game.player2 = player2;

  g_print("START\n");
}
