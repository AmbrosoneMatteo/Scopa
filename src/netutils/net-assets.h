// This header file contains definitions for structures that need to be sent over the network
// The structures are used by the functions in the serializuer.c file
#ifndef NET_ASSETS
#define NET_ASSETS
#define MAX_COMBO_SIZE 4
#define MAX_COMBOS 10

#include <stdbool.h>
#include "engine/game-assets.h"

struct NetHand {
  struct Card cards[HAND_SIZE];
  int count;
};

struct NetTable {
  struct Card cards[MAX_TABLE_SIZE];
  int count;
};

struct NetCombinationList {
  struct Card combinations[MAX_COMBOS][MAX_COMBO_SIZE];
  int combo_sizes[MAX_COMBOS];
  int count;
};

struct NetUpdatePile {
  struct Card card;
  bool is_opponent_pile;
};

struct NetPile{
  struct Card cards[DECK_SIZE];
  int count;
};

struct NetEndGameData {
  struct NetPile player_pile;
  struct NetPile opponent_pile;
  int player_scope;
  int opponent_scope;
};

#endif