#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "engine/game-assets.h"
#include "netutils/net-assets.h"
#include "netutils/serializer.h"
#include "netutils/communication.h"

void send_packet_reqcard(GOutputStream *out){
  send_packet(out, REQ_CARD, NULL, 0);
}

void send_packet_oppcard(GOutputStream *out, struct Card *card){
  send_packet(out, OPPONENT_CARD, card, sizeof(struct Card));
}

void send_packet_init(GOutputStream *out){
  send_packet(out, INIT, NULL, 0);
}

// Functions that sends to the player the player's hand after it gets serialized
void send_packet_hand(GOutputStream *out, struct Hand *hand){
  struct NetHand *net_hand = serialize_hand(hand);
  send_packet(out, SET_HAND, net_hand, sizeof(struct NetHand));
  free(net_hand);
}

// Functions that sends to the player the game table after it gets serialized
void send_packet_table(GOutputStream *out, struct Table *table){
  struct NetTable *net_table = serialize_table(table);
  send_packet(out, UPDATE_TABLE, net_table, sizeof(struct NetTable));
  free(net_table);
}

// Function that sends to the player the request to select a possible combination
void send_packet_reqcombo(GOutputStream *out, struct CombinationNode *combo_list){
  struct NetCombinationList *net_list = serialize_combination_list(combo_list);
  send_packet(out, REQ_COMBO, net_list, sizeof(struct NetCombinationList));
  free(net_list);
}

// Function that sends to the server the card played by the player
void send_packet_playcard(GOutputStream *out, struct Card *card){
  send_packet(out, PLAY_CARD, card, sizeof(struct Card));
}

// Function that sends to the client the signal to update the UI pile
// to display the last card played that took a something from the table
void send_packet_updatepile(GOutputStream *out, struct Card *card, bool is_opponent_pile){
  struct NetUpdatePile *net_pile = calloc(1, sizeof(struct NetUpdatePile));
  net_pile->card = *card;
  net_pile->is_opponent_pile = is_opponent_pile;
  send_packet(out, UPDATE_PILE, net_pile, sizeof(struct NetUpdatePile));
  free(net_pile);
}

void send_packet_playcombo(GOutputStream *out, int combo_index){
  send_packet(out, PLAY_COMBO, &combo_index, sizeof(int));
}

// Function that sends to the client the end of the game signal with the scores
void send_packet_endgame(GOutputStream *out, struct CardNode *player_pile,
  struct CardNode *opponent_pile, int player_scope, int opponent_scope){

  struct NetEndGameData *net_endgame = calloc(1, sizeof(struct NetEndGameData));
  net_endgame->player_pile = *(serialize_pile(player_pile));
  net_endgame->opponent_pile = *(serialize_pile(opponent_pile));
  net_endgame->player_scope = player_scope;
  net_endgame->opponent_scope = opponent_scope;
  send_packet(out, GAME_END, net_endgame, sizeof(struct NetEndGameData));
  free(net_endgame);
}

// Function that receives the player played card from the input stream
// and parses it to a struct Card. A pointer to the card is returned
struct Card *receive_packet_card(GInputStream *in){
  struct GamePacket header;
  struct Card *card = (struct Card *)receive_packet(in, &header);
  if(header.type != PLAY_CARD){
    g_error("Error receiving card, game aborted\n");
    return NULL;
  }
  if(card == NULL){
    g_error("Error receiving card, game aborted\n");
    return NULL;
  }
  return card;
}

int receive_packet_comboselect(GInputStream *in){
  struct GamePacket header;
  int *combo_index = (int*)receive_packet(in, &header);
  if(header.type != PLAY_COMBO){
    g_error("Error receiving combination, game aborted\n");
    return -1;
  }
  if(combo_index == NULL){
    g_error("Error receiving combination, game aborted\n");
    return -1;
  }
  return *combo_index;
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
  // Reading the received payload
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