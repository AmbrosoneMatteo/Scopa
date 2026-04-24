#ifndef GAME_HELPER
#define GAME_HELPER
#include "game-assets.h"
#include <stdbool.h>

struct Deck *deck_init(void);
void shuffle_deck(struct Deck *deck);
struct Card *draw_card(struct Deck *deck);
struct Table *table_init(struct Deck *deck);
void get_hand(struct Deck *deck, struct Hand *hand);
void remove_node (struct CardNode * node);
void delete_node (struct CardNode * node);
struct CardNode * get_node_at_index(struct CardNode * node, int index);
struct CardNode * append_card (struct CardNode * list,struct Card * card);
void append_node (struct CardNode *list, struct CardNode *node);
void send_player_card(struct Card * card, int index);
bool is_sum_inside_deck(struct Hand * player_hand, struct CombinationList * list);
struct CombinationNode * calculate_possible_combination(struct Hand * player_hand,
                                                 struct Table * table);
void print_list(struct CombinationNode * list);
int get_node_number(struct CardNode * node);
bool hand_has_card(struct Hand *hand, struct Card *card);

#endif
