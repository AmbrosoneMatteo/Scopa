#include "engine/game-assets.h"
#include <stdbool.h>

void start_local_game(int diff);
extern struct Deck * deck;
extern struct CardNode * memorized_card;
extern struct Hand * bot_hand;
extern struct Table * table;
void player_play_card(int player_card_index, int table_card_index);

extern int difficulty;
