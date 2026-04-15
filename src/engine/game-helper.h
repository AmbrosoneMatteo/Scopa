#ifndef GAME_HELPER
#define GAME_HELPER
#include "game-assets.h"

struct Deck *deck_init(void);
void shuffle_deck(struct Deck *deck);
struct Card *draw_card(struct Deck *deck);
struct Table *table_init(struct Deck *deck);
struct Hand * get_hand(struct Deck *deck);
void remove_node (struct CardNode * node);
void append_node (struct CardNode * list,struct Card * card);

#endif
