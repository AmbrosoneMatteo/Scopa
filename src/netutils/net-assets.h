// This header file contains definitions for structures that need to be sent over the network
// The structures are used by the functions in the serializuer.c file
#ifndef NET_ASSETS
#define NET_ASSETS

#include "engine/game-assets.h"

struct NetHand {
  struct Card cards[HAND_SIZE];
  int count;
};

struct NetTable {
  struct Card cards[MAX_TABLE_SIZE];
  int count;
};

#endif