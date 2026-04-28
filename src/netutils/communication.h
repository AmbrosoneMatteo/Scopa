#ifndef COMMUNICATION_H
#define COMMUNICATION_H

// Enum that rapresents the type of the packet
enum MsgType {
  INIT, // Indicates the start of the game
  REQ_CARD, // Request the player for a card
  PLAY_CARD, // Card played by the player
  SET_HAND, // Set the player's hand
  UPDATE_TABLE, // Update the table
  OPPONENT_CARD, // Send to the player the card played by the opponent
  REQ_COMBO, // Request the player to select a possible combination when there is more than one possible
  PLAY_COMBO // Combination played by the player
};

// Structure used as header of the packets that are sent from and to the server
struct GamePacket {
  enum MsgType type;
  int payload_length;
};

void send_packet(GOutputStream *out, enum MsgType type, void *payload, int len);
void* receive_packet(GInputStream *in, struct GamePacket *out_header);
void send_packet_hand(GOutputStream *out, struct Hand *hand);
void send_packet_table(GOutputStream *out, struct Table *table);
void send_packet_reqcard(GOutputStream *out);
void send_packet_oppcard(GOutputStream *out, struct Card *card);
void send_packet_reqcombo(GOutputStream *out, struct CombinationNode *combo_list);
struct Card *receive_packet_card(GInputStream *in);
int receive_packet_comboselect(GInputStream *in);

#endif
