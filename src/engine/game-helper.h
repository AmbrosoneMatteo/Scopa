#ifndef GAME_HELPER
#define GAME_HELPER
#include "game-assets.h"
#include <stdbool.h>

struct Deck *deck_init(void);
void shuffle_deck(struct Deck *deck);
struct Card *draw_card(struct Deck *deck);
struct Table *table_init(struct Deck *deck);
struct Hand * get_hand(struct Deck *deck);
void remove_node (struct CardNode * node);
void delete_node (struct CardNode * node);
struct CardNode * get_node_at_index(struct CardNode * node, int index);
void append_card (struct CardNode * list,struct Card * card);
void append_node (struct CardNode *list, struct CardNode *node);
void send_player_card(struct Card * card, int index);
bool is_sum_inside_deck(struct Hand * player_deck, struct CardNode * list);

#endif
