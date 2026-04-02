#include <stdlib.h>
#include <stdbool.h>

#include "game-helper.h"
#include "engine/game-assets.h"

// Fucntion that initializes a deck structure with all the cards
// used in the Scopa game. The cards are inserted in incremental order.
// Returned is a pointer to the deck structure
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
  deck->top = &deck->cards[DECK_SIZE - 1]; // Resetting the top of the deck
}

// Function that draws a card from the top of the deck
// Returned is the pointer to the card
struct Card *draw_card(struct Deck *deck){
  if(deck->top >= &deck->cards[0]){
    struct Card *card = deck->top;
    deck->top--;
    return card;
  }
  return NULL;
}

// Function that initializes the table with 4 cards
// Returned is the pointer to the table structure
struct Table *table_init(struct Deck *deck){
  struct Table *table = calloc(1, sizeof(struct Table));
  if(table == NULL){
    return NULL;
  }
  
  for(int i = 0; i < 4; i++){
    table->cards[i] = *(draw_card(deck));
  }
  table->count = 4; // Initial size of the table
  return table;
}

// Function that gets a new 3 card hand for the player
// Returned is the pointer to the hand structure
struct Hand *get_hand(struct Deck *deck, struct Hand *hand){
  for(int i = 0; i < HAND_SIZE; i++){
    hand->cards[i] = *(draw_card(deck));
  }
  hand->count = HAND_SIZE;
  return hand;
}

// Function that checks if the card played by the player is effectively
// in the player's hand
bool hand_has_card(struct Hand *hand, struct Card *card){
  for(int i = 0; i < hand->count; i++){
    if(hand->cards[i].value == card->value && hand->cards[i].suit == card->suit){
      return true;
    }
  }
  return false;
}