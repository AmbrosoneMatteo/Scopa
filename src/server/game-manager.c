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

  send_packet_hand(player1_out, player1_hand);
  send_packet_hand(player2_out, player2_hand);
  send_packet_table(player1_out, table);
  send_packet_table(player2_out, table);

  // Game loop
  while(deck->count >= 6){ // There are enough cards to deal the last hand
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
      send_packet_table(player1_out, table);
      send_packet_table(player2_out, table);
    }
    // TODO: Update user hands
  }

  g_print("START\n");
}