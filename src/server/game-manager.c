#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "engine/game-assets.h"
#include "engine/game-helper.h"
#include "netutils/communication.h"

#include "game-manager.h"

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
  get_hand(deck, player1_hand);
  get_hand(deck, player2_hand);
  bool player1_turn = true;

  struct Table *table = table_init(deck);

  send_packet(player1_out, SET_HAND, player1_hand, sizeof(struct Hand));
  send_packet(player2_out, SET_HAND, player2_hand, sizeof(struct Hand));
  send_packet(player1_out, UPDATE_TABLE, table, sizeof(struct Table));
  send_packet(player2_out, UPDATE_TABLE, table, sizeof(struct Table));

  // Game loop
  while(deck->top >= &deck->cards[0]){
    for(int i = 0; i < 6; i++){
      struct GamePacket header;
      // TODO: Check header
      if(player1_turn) {
        send_packet(player1_out, REQ_CARD, NULL, 0);
        struct Card *card = (struct Card *)receive_packet(player1_in, &header);
        if(card == NULL){
          g_error("Error receiving card, game aborted\n");
          return;
        }
        g_print("Player 1 played: %d of %d\n", card->value, card->suit); // Temp debug info

        if(!hand_has_card(player1_hand, card)){
          g_error("Error: card played not found in user's hand, game aborted\n");
          return;
        }

        // TODO: Calculate what the player takes from the table

        send_packet(player2_out, OPPONENT_CARD, card, sizeof(struct Card));
        player1_turn = false;
      }else{
        send_packet(player2_out, REQ_CARD, NULL, 0);
        struct Card *card = (struct Card *)receive_packet(player2_in, &header);
        if(card == NULL){
          g_error("Error receiving card, game aborted\n");
          return;
        }
        g_print("Player 2 played: %d of %d\n", card->value, card->suit); // Temp debug info

        if(!hand_has_card(player2_hand, card)){
          g_error("Error: card played not found in user's hand, game aborted\n");
          return;
        }

        // TODO: Calculate what the player takes from the table

        send_packet(player1_out, OPPONENT_CARD, card, sizeof(struct Card));
        player1_turn = true;
      }
      send_packet(player1_out, UPDATE_TABLE, table, sizeof(struct Table));
      send_packet(player2_out, UPDATE_TABLE, table, sizeof(struct Table));
    }
    // TODO: Update user hands
  }

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

// Function that waits for a packet from the client
// The function takes as parameters the input stream and a pointer
// to the header of the packet received
void* receive_packet(GInputStream *in, struct GamePacket *out_header) {
  gsize bytes_read;
  GError *error = NULL;

  // Read the header packet from the stream (8 bytes)
  gboolean success = g_input_stream_read_all(
    in, 
    out_header, 
    sizeof(struct GamePacket), 
    &bytes_read, 
    NULL, 
    &error
  );

    if (!success) {
      if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
        g_error("Player timeout exceeded\n");
      }
      g_error_free(error);
      return NULL;
    }

  void *payload = NULL;
  // Reading the payload (played card)
  if (out_header->payload_length > 0) {
    payload = malloc(out_header->payload_length);
    if(payload == NULL){
      return NULL;
    }
    success = g_input_stream_read_all(
      in, 
      payload, 
      out_header->payload_length, 
      &bytes_read, 
      NULL, 
      &error
    );

    if (!success) {
      g_free(payload);
      return NULL;
    }
  }
  return payload;
}