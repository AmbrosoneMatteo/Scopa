#ifndef GAME_HELPER
#define GAME_HELPER
#include "game-assets.h"
#include <stdbool.h>

struct Deck *deck_init(void);
void shuffle_deck(struct Deck *deck);
struct Card *draw_card(struct Deck *deck);
struct Table *table_init(struct Deck *deck);
struct Table *table_init_display(struct Deck *deck);
void get_hand(struct Deck *deck, struct Hand *hand);
void remove_node (struct CardNode * node);
void delete_node (struct CardNode * node);
struct CardNode * get_node_at_index(struct CardNode * node, int index);
struct CardNode * append_card (struct CardNode * list,struct Card * card);
void append_node (struct CardNode *list, struct CardNode *node);
bool is_sum_inside_deck(struct Hand * player_hand, struct CombinationList * list);
struct CombinationNode *get_combinations_for_card(struct Card * card, struct Table * table);
int get_combo_length(struct CombinationList *list);
struct CombinationList *determine_auto_take(struct CombinationNode *possibilities);
void remove_combination_from_table(struct Table *table, struct CombinationList *list, struct CardNode **player_pile);
void remove_card_from_hand(struct Hand *hand, struct Card *card);
struct CombinationList *get_combination_at_index(struct CombinationNode *node, int index);
void print_list(struct CombinationNode * list);
int get_node_number(struct CardNode * node);
bool hand_has_card(struct Hand *hand, struct Card *card);
void local_play_card(struct Hand* hand,struct Card *card, struct CardNode* pile,
                      struct CombinationNode* combinations,
                      struct Table *table,
                      struct Deck *deck,
                      int *scopa_counter);
#endif
