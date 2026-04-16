#ifndef COMMUNICATION_H
#define COMMUNICATION_H
enum MsgType {
  INIT,
  REQ_CARD,
  SET_HAND,
  UPDATE_TABLE,
  OPPONENT_CARD
};

struct GamePacket {
  enum MsgType type;
  int payload_length;
};

#endif
