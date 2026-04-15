#include "engine/game-assets.h"
#include <stdbool.h>

void start_local_game(int difficulty);
bool has_card (struct Hand * hand,struct Card * card);
extern struct Deck * deck;
extern struct CardNode * memorized_card;
extern struct Hand * bot_hand;
extern struct Table * table;
