#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "engine/game-assets.h"
#include "scopa-application.h"
#include "scopa-window.h"
#include "game-helper.h"

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
    return node->previous;
}

struct CardNode * get_node_at_index(struct CardNode * node, int index) {
    struct CardNode * current = node;
    int i = 0;
    while(i<index && current->next!=NULL) {
        i++;
        current = current->next;
    }
    if (i==index) {
        return current;
    }
    return NULL;
}

// checks if a card can be placed on the table to take something
bool can_place_card(struct Card * card, struct Table * table) {
    struct CardNode * l_node = table->node;
    while (l_node->next != NULL) {
        if (l_node->card->value == card->value)
            return true;
        l_node = l_node->next;
    }

    return false;
}

// cycles through the linked list and returns an array with the values in it
int * node_to_array(struct CardNode * node) {
    int* array = (int*)malloc(get_node_number (node) * sizeof(int));
    if (array == NULL) {
        g_print("Memory allocation failed!\n");
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

bool is_sum_inside_deck(struct Hand * player_hand, struct CombinationList * list) {
    if (list == NULL)
        return false;

    int sum = 0;
    do {
        g_print("Calculating...| list address: %p\n", list);
        struct CardNode * node = list->node;
        struct Card * card = node->card;
        sum+=card->value;
        if (sum>10)
            return false;
        list = list->next;
    } while (list != NULL);

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

/**
 * This function as described in the name it prints a linked list in the terminal
 * This function is only for debugging purposes and must not be used in a
 * production environment
 * */
void print_list(struct CombinationNode * list) {
    do {
        struct CombinationList * current = list->list;
        if (current != NULL) {
            do {
                struct Card * current_card = current->node->card;

                g_print("%d%c -", current_card->value,
                        suit_strings[current_card->suit]);
                current = current->next;
            } while (current !=NULL);
        } else {
            g_print("empty list of cards\n");
        }
        printf("\n");
        list = list->next;
    } while(list != NULL);
}

void append_combination_list(struct CombinationList * combinations, struct CardNode * card_node) {
    struct CombinationList * node = (struct CombinationList *)malloc(
                                          sizeof(struct CombinationList));
    node->node = card_node;
    node->next = NULL;
    while(combinations->next != NULL)
        combinations = combinations->next;
    combinations->next = node;
}

void append_combination(struct CombinationNode * combinations, struct CombinationList * list) {
    struct CombinationNode * node = (struct CombinationNode *)malloc(
                                          sizeof(struct CombinationNode));
    node->list = list;
    node->next = NULL;
    while(combinations->next != NULL)
        combinations = combinations->next;
    combinations->next = node;
}

/*
 * This function returns all the possible legal combinations that the user
 * can take from the table, by comparing every possible card combination
 * against the player's deck. The function to get the power set is inspired
 * by this implementation of a powerset in C:
 * https://learnprogramming.in.net/power-set-generator-in-c/
 * */
struct CombinationNode * calculate_possible_combination(struct Hand * player_hand,
                                                 struct Table * table) {
    struct CardNode * table_cards = table->node;
    int list_size = get_node_number (table_cards);
    unsigned power_set_size = upow(2, list_size);

    struct CombinationNode * combinations = NULL;
    int index = 0;

    for (unsigned int i = 0; i < power_set_size; i++) {
        //This linked list temporarily stores the nodes of the combination,
        // if the combination is legal this is then added to the list
        struct CombinationList * tmp_list = NULL;
        for (int j = 0; j < list_size; j++) {
            // Check if jth element is included in the current subset
            if (i & (1 << j)) {
                 struct CardNode * node = get_node_at_index (table_cards, j);
                 if (tmp_list == NULL && node != NULL) {
                      tmp_list = (struct CombinationList *)malloc(
                                              sizeof (struct CombinationList));
                      tmp_list->node = node;
                      tmp_list->next = NULL;
                 }
                 else if(node != NULL)
                    append_combination_list (tmp_list, node);
                 g_print("Card node address: %p\n", node);
            }
        }
        if(is_sum_inside_deck(player_hand, tmp_list)) {
              if (combinations == NULL) {
                  combinations = (struct CombinationNode *)malloc(sizeof
                                                  (struct CombinationNode));
                  combinations->list = tmp_list;
                  combinations->next = NULL;
              } else {
                  append_combination(combinations, tmp_list);
              }
        }
    }

    return combinations;
}

// Function that returns all the combinations that a single card can take from the table
// The function creates a dummy hand with only the card played by the user and uses
// the calculate_possible_combination function
struct CombinationNode *get_combinations_for_card(struct Card * card, struct Table * table) {
    struct Hand dummy_hand;
    dummy_hand.cards[0] = card;
    dummy_hand.cards[1] = NULL;
    dummy_hand.cards[2] = NULL;
    dummy_hand.count = 1;

    return calculate_possible_combination(&dummy_hand, table);
}

// Function that returns how many cards does a possible combination have
int get_combo_length(struct CombinationList *list) {
    int count = 0;
    while(list != NULL){
        count++;
        list = list->next;
    }
    return count;
}

// Function to determinate if in all possible combinations there is one that must be taken
// For example the user is forced to take the direct combination with 1 card if available
// Returns the combination that must be taken or NULL otherwise
struct CombinationList *determine_auto_take(struct CombinationNode *possibilities) {
    if(possibilities == NULL){
        return NULL;
    }
    struct CombinationNode *current_node = possibilities;
    int total_options = 0;
    struct CombinationList *current_combo = NULL;

    while (current_node != NULL) {
        total_options++;
        current_combo = current_node->list;
        if (get_combo_length(current_combo) == 1) {
            return current_combo; // returning combination to take with 1 card
        }
        current_node = current_node->next;
    }

    // If there is only 1 option available we do not need to ask the user which combo to take
    if (total_options == 1) {
        return current_combo;
    }
    return NULL; // The user needs to be asked what combo to take
}

// Function that removes all the CardNodes of a combination from the table and adds them to the player's pile
void remove_combination_from_table(struct Table *table, struct CombinationList *list, struct CardNode **player_pile) {
    while(list != NULL){
        struct CardNode *node = list->node;
        struct CombinationList *tmp = list;
        list = list->next;
        if(table->node == node){
            // Shifting the table starting node if the node to remove is the first one
            table->node = node->next;
        }
        remove_node(node);
        table->count--;
        node->next = NULL;
        node->previous = NULL;
        if(*player_pile == NULL){
            // The player is collecting their first card so the pile ie empty
            *player_pile = node;
        }else{
            append_node(*player_pile, node);
        }
        free(tmp);
    }
}

void remove_card_from_hand(struct Hand *hand, struct Card *card) {
    for(int i = 0; i < HAND_SIZE; i++){
        if(hand->cards[i] != NULL){
            if(hand->cards[i]->value == card->value && hand->cards[i]->suit == card->suit){
                hand->cards[i] = NULL;
                hand->count--;
            }
        }
    }
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
    printf("%p\n", table->node);

    if (table->node == NULL){
        table->node = append_card (table->node, card);
    }else{
        append_card (table->node, card);
    }
  }

  table->count = TABLE_SIZE; // Initial size of the table
  return table;
}

// Function that initializes the table using the table_init function
// and then displays the cards on the GUI
struct Table *table_init_display(struct Deck *deck){
    struct Table *table = table_init(deck);
    struct CardNode * l_node = table->node;
    int i = 0;
    while(l_node != NULL){
        struct Card * card = l_node->card;
            char *path;
        asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png",
                        card->value,
                        suit_strings[card->suit]);
        place_card_on_table (main_window,
                                path, i);
        i++;
        l_node = l_node->next;
    }
    return table;
}

struct CardNode * append_card (struct CardNode * list,struct Card * card) {
    if (list == NULL) {
        struct CardNode * node = (struct CardNode *)malloc(sizeof(struct CardNode));
        node->card = card;
        node->next = NULL;
        node->previous = NULL;
        return node;
    } else {
        while (get_next_node (list) != NULL)
            list = get_next_node (list); // get to the last card in the linked lisk
        struct CardNode * node = (struct CardNode *)malloc(sizeof(struct CardNode));
        node->card = card;
        node->next = NULL;
        node->previous = list;
        list->next = node;
    }

    return NULL;
}

void append_node (struct CardNode * list,struct CardNode * node) {
    while (list->next != NULL)
        list = get_next_node (list); // get to the last card in the linked lisk
    list->next = node;
}

void send_player_card(struct Card * card, int index) {
  place_player_card (main_window,  card, index);
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
    if (node->next!=NULL && node->previous!=NULL) {
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
void get_hand(struct Deck *deck, struct Hand *hand){
  for(int i = 0; i < HAND_SIZE; i++){
    hand->cards[i] = draw_card(deck);
  }
  hand->count = HAND_SIZE;
}

// Function that checks if the card played by the player is effectively
// in the player's hand
bool hand_has_card(struct Hand *hand, struct Card *card){
  for(int i = 0; i < HAND_SIZE; i++){
    if(hand->cards[i] != NULL){
      if(hand->cards[i]->value == card->value && hand->cards[i]->suit == card->suit){
        return true;
      }
    }
  }
  return false;
}
