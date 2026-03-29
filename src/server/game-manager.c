#include <glib.h>
#include <gio/gio.h>

#include "game-manager.h"
#include "game-helper.h"
#include "game/game-assets.h"
#include "netutils/communication.h"

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

  // Deck initialization
  struct Deck *deck = deck_init();
  shuffle_deck(deck);

  // Player's hands initialization
  struct Hand *player1_hand = malloc(sizeof(struct Hand));
  struct Hand *player2_hand = malloc(sizeof(struct Hand));
  player1_hand = get_hand(deck, player1_hand);
  player2_hand = get_hand(deck, player2_hand);

  struct Table *table = table_init(deck);

  send_packet(player1_out, SET_HAND, player1_hand, sizeof(struct Hand));
  send_packet(player2_out, SET_HAND, player2_hand, sizeof(struct Hand));
  send_packet(player1_out, UPDATE_TABLE, table, sizeof(struct Table));
  send_packet(player2_out, UPDATE_TABLE, table, sizeof(struct Table));

  g_print("START\n");
}

// Function that sends a packet to the client through the output stream
// The function takes as parameters the output stream, the type of the packet,
// the payload and the length of the payload
void send_packet(GOutputStream *out, enum MsgType type, void *payload, int payload_length) {
  struct GamePacket header = { .type = type, .payload_length = payload_length };
  // Sending the header (8 bytes) before the payload
  g_output_stream_write_all(out, &header, sizeof(header), NULL, NULL, NULL);

  // Sending the payload if set
  if (payload_length > 0 && payload != NULL) {
    g_output_stream_write_all(out, payload, payload_length, NULL, NULL, NULL);
  }
}