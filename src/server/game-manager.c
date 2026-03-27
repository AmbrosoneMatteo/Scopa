#include <glib.h>
#include <gio/gio.h>

#include "game-manager.h"
#include "game-helper.h"
#include "game/game-assets.h"

struct GameContext {
  GSocketConnection *player1;
  GSocketConnection *player2;
};
struct GameContext game = {NULL, NULL};

void start_game(GSocketConnection *player1, GSocketConnection *player2){
  game.player1 = player1;
  game.player2 = player2;

  GInputStream *player1_in = g_io_stream_get_input_stream(G_IO_STREAM (player1));
  GInputStream *player2_in = g_io_stream_get_input_stream(G_IO_STREAM (player2));
  GOutputStream *player1_out = g_io_stream_get_output_stream(G_IO_STREAM (player1));
  GOutputStream *player2_out = g_io_stream_get_output_stream(G_IO_STREAM (player2));

  struct Deck *deck = deck_init();
  shuffle_deck(deck);

  g_print("START\n");
}
