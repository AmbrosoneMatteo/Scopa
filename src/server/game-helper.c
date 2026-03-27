#include <stdlib.h>

#include "game-helper.h"
#include "game/game-assets.h"

// Fucntion that initializes a deck structure with all the cards
// used in the Scopa game. The cards are inserted in incremental order.
struct Deck *deck_init(void){
  struct Deck *deck = malloc(sizeof(struct Deck));
  if(deck == NULL){
    return NULL;
  }
  int current_index = 0;
  for(int i = 1; i <= 10; i++){
    for(int j = 0; j < 4; j++){
      struct Card card = {.value = i, .suit = j};
      deck->cards[current_index] = card;
      current_index++;
    }
  }
  deck->top = &deck->cards[current_index - 1];
  return deck;
}

// Function that suffels the deck using the Fisher–Yates shuffle Algorithm
// that gives the same probability to every possible permutation.
// (see: https://www.geeksforgeeks.org/dsa/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/)
void shuffle_deck(struct Deck *deck){
  for(int i = DECK_SIZE-1; i > 0; i--){
    int j = rand() % (i+1);

    struct Card temp = deck->cards[i];
    deck->cards[i] = deck->cards[j];
    deck->cards[j] = temp;
  }
}