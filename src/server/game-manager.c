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
  
  GInputStream *in_streams[2] = {player1_in, player2_in};
  GOutputStream *out_streams[2] = {player1_out, player2_out};
  struct Hand *hands[2] = {player1_hand, player2_hand};
  // Structures to store the cards collected by each player during the game
  struct CardNode *player_piles[2] = {NULL, NULL};
  int scope_counter[2] = {0, 0};

  // Game loop
  while(deck->count >= 6){ // There are enough cards to deal the last hand
    send_packet_hand(player1_out, player1_hand);
    send_packet_hand(player2_out, player2_hand);
    send_packet_table(player1_out, table);
    send_packet_table(player2_out, table);
    for(int i = 0; i < 6; i++){
      // Calculating who is the current player
      int current_player = (player1_turn) ? 0 : 1;
      int opponent = (player1_turn) ? 1 : 0;

      // Receiving player's card
      send_packet_reqcard(out_streams[current_player]);
      struct Card *card = receive_packet_card(in_streams[current_player]);
      if(card == NULL){
        g_error("Error receiving card, game aborted\n");
        return;
      }
      g_print("Player played: %d of %d\n", card->value, card->suit); // Temp debug info
      if(!hand_has_card(hands[current_player], card)){
        g_error("Error: card played not found in user's hand, game aborted\n");
        return;
      }

      // Calculate what the player takes from the table
      struct CombinationNode *combinations = get_combinations_for_card(card, table);
      if(combinations != NULL){
        struct CombinationList *auto_take = determine_auto_take(combinations);
        if(auto_take != NULL) {
          remove_combination_from_table(table, auto_take, &player_piles[current_player]);
          remove_card_from_hand(hands[current_player], card);
          // Adding the card that the player had in the hand to their pile
          append_card(player_piles[current_player], card); 
        }else{
          // Ask the user what combination he wants to get
          send_packet_reqcombo(out_streams[current_player], combinations);
          int combo_index = receive_packet_comboselect(in_streams[current_player]);
          struct CombinationList *combo = get_combination_at_index(combinations, combo_index);
          if(combo == NULL){
            g_error("Error receiving combination, game aborted\n");
            return;
          }
          remove_combination_from_table(table, combo, &player_piles[current_player]);
          remove_card_from_hand(hands[current_player], card);
          append_card(player_piles[current_player], card);
        }
        if(table->count == 0){
          // If the palyer clears the table, they get a scopa point
          scope_counter[current_player]++;
        }
      }else{
        // The player cannot take anything from the table
        // Adding the card the table
        if(table->node == NULL){
          table->node = append_card(NULL, card);
        }else{
          append_card(table->node, card);
        }
        table->count++;
        remove_card_from_hand(hands[current_player], card);
      }

      send_packet_oppcard(out_streams[opponent], card);
      send_packet_hand(out_streams[current_player], hands[current_player]);

      send_packet_table(out_streams[current_player], table);
      send_packet_table(out_streams[opponent], table);
      player1_turn = !player1_turn;
    }

    // Update players hands with new cards
    get_hand(deck, player1_hand);
    get_hand(deck, player2_hand);
  }

  g_print("START\n");
}