#ifndef COMMUNICATION_H
#define COMMUNICATION_H

// Enum that rapresents the type of the packet
enum MsgType {
  INIT, // Indicates the start of the game
  REQ_CARD, // Request the player for a card
  PLAY_CARD, // Card played by the player
  SET_HAND, // Set the player's hand
  UPDATE_TABLE, // Update the table
  OPPONENT_CARD // Send to the player the card played by the opponent
};

// Structure used as header of the packets that are sent from and to the server
struct GamePacket {
  enum MsgType type;
  int payload_length;
};

void send_packet(GOutputStream *out, enum MsgType type, void *payload, int len);
void* receive_packet(GInputStream *in, struct GamePacket *out_header);

#endif
