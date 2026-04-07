#ifndef GAME_HELPER
#define GAME_HELPER

struct Deck *deck_init(void);
void shuffle_deck(struct Deck *deck);
struct Card *draw_card(struct Deck *deck);
struct Table *table_init(struct Deck *deck);
void *get_hand(struct Deck *deck, struct Hand *hand);
void remove_node (struct CardNode * node);
void append_node (struct CardNode * list,struct Card * card);

#endif
