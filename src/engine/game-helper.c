#include <stdlib.h>
#include <stdio.h>

#include "game-helper.h"
#include "engine/game-assets.h"
#include "scopa-application.h"
#include "scopa-window.h"

// Function that initializes a deck structure with all the cards
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
  deck->count = DECK_SIZE;
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

// returns the number of nodes in the array list
int get_node_number(struct CardNode * node) {
    if (node==NULL)
        return 0;
    int out = 1;
    while (node->next!=NULL) {
         node = node->next;
          out++;
    }

    return out;
}

// Function that draws a card from the top of the deck
// Returned is the pointer to the card
struct Card *draw_card(struct Deck *deck){
  if(deck->count>0){
    struct Card *card = &deck->cards[deck->count-1];
    deck->count--;
    return card;
  }
  return NULL;
}

struct CardNode * get_next_node(struct CardNode * node) {
    return node->next;
}

struct CardNode * get_previous_node(struct CardNode * node) {
    return node->next;
}

struct CardNode * get_node_at_index(struct CardNode * node, int index) {
    struct CardNode * current = node;
    int i = 0;
    while(i<index && current->next!=NULL) {
        i++;
        current = current->next;
    }
    if (i==index && current->next != NULL) {
        current = current->next;
    } else {
       current = NULL;
    }
    return current;
}


// Function that initializes the table with TABLE_SIZE cards
// Returned is the pointer to the table structure
struct Table *table_init(struct Deck *deck){
  struct Table *table = calloc(1, sizeof(struct Table));
  if(table == NULL){
    return NULL;
  }
  
  for(int i = 0; i < TABLE_SIZE; i++){
      struct Card * card = draw_card(deck);
      append_node (table->node, card);
      char *path;
      asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png",
                      card->value,
                      suit_strings[card->suit]);
      place_card_on_table (main_window,
                             path, i);
  }
  table->count = TABLE_SIZE; // Initial size of the table
  return table;
}

void append_node (struct CardNode * list,struct Card * card) {
    if (list == NULL) {
        struct CardNode node = {card, NULL, NULL};
        list = &node;
    } else {
        struct CardNode * current = NULL;
        while ((current = get_next_node (current)) != NULL)
            ; // get to the last card in the linked lisk
        struct CardNode node = {card, NULL, current};
        current->next = &node;
    }
}

void send_player_card(struct Card * card, int index) {
  char *path;
  asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png", card->value,
              suit_strings[card->suit]);

  place_player_card (main_window, path, index);
}

/**
 * this method removes a node from the linked list using the function
 * remove_node, and then proceeds to free the pointer to the card, and the pointer
 * to the node
 */
void delete_node (struct CardNode * node) {
    remove_node (node);
    free(node->card);
    free(node);
}

//removes a node from the linked list, and updates the chain
void remove_node (struct CardNode * node) {
    if (node->next!=NULL && node->previous==NULL) {
        node->next->previous = node->previous;
        node->previous->next = node->next;
    } else if (node->next!=NULL && node->previous==NULL) {
        //In this case the node is at the start of the chain
        node->next->previous = NULL;
    } else if (node->next==NULL && node->previous!=NULL) {
        //In this case the node is at the end of the chain
        node->previous->next = NULL;
    }
}

// Function that gets a new 3 card hand for the player
// Returned is the pointer to the hand structure
struct Hand * get_hand(struct Deck *deck)  {
    struct Hand *hand = malloc (sizeof(struct Hand));
    for(int i = 0; i < HAND_SIZE; i++){
        hand->cards[i] = draw_card(deck);
    }
    hand->count = HAND_SIZE;
    return hand;
}
