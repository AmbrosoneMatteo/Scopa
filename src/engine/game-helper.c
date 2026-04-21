#include <stdlib.h>
#include <stdio.h>

#include "game-helper.h"
#include "engine/game-assets.h"
#include "scopa-application.h"
#include "scopa-window.h"

int * node_to_array(struct CardNode * node);
bool can_place_card(struct Card * card, struct Table * table);

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

// checks if a card can be placed on the table to take something
bool can_place_card(struct Card * card, struct Table * table) {
    struct CardNode * l_node = table->node;
    while (l_node->next != NULL) {
        if (l_node->card->value == card->value)
            return true;
    }

    return false;
}

// cycles through the linked list and returns an array with the values in it
int * node_to_array(struct CardNode * node) {
    int* array = (int*)malloc(get_node_number (node) * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        exit(1); // Exit the program if allocation fails
    }

    int index = 0;
    while (node->next!=NULL) {
        array[index] = node->card->value;
        index++;
        node = node->next;
    }

    return array;
}

bool is_sum_inside_deck(struct Hand * player_hand, struct CardNode * list) {
    if (list == NULL)
        return false;

    int sum = 0;
    struct CardNode * current = list;
    while (current->next != NULL) {
        sum+=current->card->value;
        if (sum>10)
            return false;
        current = current->next;
    }

    for(int i = 0; i<3 && player_hand->cards[i]!=NULL; i++) {
        if(player_hand->cards[i]->value == sum)
            return true;
    }

    return false;
}

// Primitive implementation of the power function returning an unsigned integer,
// because the linker can't find the pow function in GLIBC
unsigned upow(unsigned base, int exponent) {
    unsigned out = 1;
    while(exponent>0) {
        out*=base;
        exponent--;
    }

    return out;
}

/*
 * This function returns all the possible legal combinations that the user
 * can take from the table, by comparing every possible card combination
 * against the player's deck. The function to get the power set is inspired
 * by this implementation of a powerset in C:
 * https://learnprogramming.in.net/power-set-generator-in-c/
 * */
struct CardNode ** calculate_possible_combination(int* array, int size,
                                                 struct Hand * player_hand,
                                                 struct Table * table) {
    struct CardNode * table_cards = table->node;
    int list_size = get_node_number (table_cards);
    unsigned power_set_size = upow(2, list_size);

    struct CardNode * node[list_size];
    int index = 0;

    for (unsigned int i = 0; i < power_set_size; i++) {
        //This linked list temporarily stores the nodes of the combination,
        // if the combination is legal this is then added to the list
        struct CardNode * tmp_list = NULL;
        for (int j = 0; j < list_size; j++) {
            // Check if jth element is included in the current subset
            if (i & (1 << j)) {
                 append_node (tmp_list, get_node_at_index (table_cards, j));
            }
        }
        if(is_sum_inside_deck(player_hand, tmp_list)) {

        }
    }


    return node;
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
      append_card (table->node, card);
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

void append_card (struct CardNode * list,struct Card * card) {
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

void append_node (struct CardNode * list,struct CardNode * node) {
    if (list == NULL) {
        list = node;
    } else {
        struct CardNode * current = NULL;
        while ((current = get_next_node (current)) != NULL)
            ; // get to the last card in the linked lisk
        current->next = node;
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
