#include <stdio.h>
#include <stdlib.h>

#include "netutils/net-assets.h"
#include "engine/game-helper.h"
#include "netutils/serializer.h"

// Function that serializes the Hand structure to a structure that
// does not use pointers so that can be send over the network
struct NetHand *serialize_hand(struct Hand * hand){
  struct NetHand *net_hand = calloc(1, sizeof(struct NetHand));
  int counter = 0;
  for(int i = 0; i < HAND_SIZE; i++){
    if(hand->cards[i] != NULL){
      net_hand->cards[counter] = *hand->cards[i];
      counter++;
    }
  }
  net_hand->count = counter;
  return net_hand;
}

// Function that serializes the Table structure to a structure that
// does not use pointers so that can be send over the network
struct NetTable *serialize_table(struct Table * table){
  struct NetTable *net_table = calloc(1, sizeof(struct NetTable));
  struct CardNode *node = table->node;
  int counter = 0;
  while(node != NULL){
    net_table->cards[counter] = *(node->card);
    node = node->next;
    counter++;
  }
  net_table->count = counter;
  return net_table;
}

// Function that serializes the CombinationNode structure to a structure that
// does not use pointers so that can be send over the network
struct NetCombinationList *serialize_combination_list(struct CombinationNode *list){
  struct NetCombinationList *net_list = calloc(1, sizeof(struct NetCombinationList));
  int combo_counter = 0;
  while(list != NULL && combo_counter < MAX_COMBOS){
    struct CombinationList *combo_list = list->list;
    int combo_length = 0;
    while(combo_list != NULL && combo_length < MAX_COMBO_SIZE){
      struct CardNode *node = combo_list->node;
      net_list->combinations[combo_counter][combo_length] = *(node->card);
      combo_list = combo_list->next;
      combo_length++;
    }
    net_list->combo_sizes[combo_counter] = combo_length;
    list = list->next;
    combo_counter++;
  }
  net_list->count = combo_counter;
  return net_list;
}

// Function that serializes the player's pile to a structure that
// does not use pointers so that can be send over the network
struct NetPile *serialize_pile(struct CardNode *pile){
  struct NetPile *net_pile = calloc(1, sizeof(struct NetPile));
  struct CardNode *node = pile;
  int counter = 0;
  while(node != NULL){
    net_pile->cards[counter] = *(node->card);
    node = node->next;
    counter++;
  }
  net_pile->count = counter;
  return net_pile;
}

// Function that deserializes the NetHand structure received from
// the network to a Hand structure
struct Hand *deserialize_hand(struct NetHand *net_hand){
  struct Hand *hand = calloc(1, sizeof(struct Hand));
  if(hand == NULL){
    return NULL;
  }
  for(int i = 0; i < net_hand->count; i++){
    struct Card *card = malloc(sizeof(struct Card));
    *card = net_hand->cards[i];
    hand->cards[i] = card;
  }
  hand->count = net_hand->count;
  return hand;
}

// Function that deserializes the NetTable structure received from
// the network to a Table structure
struct Table *deserialize_table(struct NetTable *net_table){
  struct Table *table = calloc(1, sizeof(struct Table));
  if(table == NULL){
    return NULL;
  }
  for(int i = 0; i < net_table->count; i++){
    struct Card *card = malloc(sizeof(struct Card));
    *card = net_table->cards[i];
    if (table->node == NULL){
      table->node = append_card(table->node, card);
    }else{
      append_card(table->node, card);
    }
  }
  table->count = net_table->count;
  return table;
}

// Function that deserializes the NetCombinationList structure received from
// the network to a CombinationNode structure
struct CombinationNode *deserialize_combination_list(struct NetCombinationList *net_list) {
  struct CombinationNode *list_head = calloc(1, sizeof(struct CombinationNode));
  if(list_head == NULL){
    return NULL;
  }
  struct CombinationNode *current_list = list_head;
  for(int i = 0; i < net_list->count; i++){
    struct CombinationList *combo_head = NULL;
    struct CombinationList *current_combo = NULL;
    for(int j = 0; j < net_list->combo_sizes[i]; j++){
      struct Card *card = calloc(1, sizeof(struct Card));
      *card = net_list->combinations[i][j];
      struct CardNode *c_node = calloc(1, sizeof(struct CardNode));
      c_node->card = card;
      struct CombinationList *new_cl = calloc(1, sizeof(struct CombinationList));
      new_cl->node = c_node;
      if(combo_head == NULL){
        combo_head = new_cl;
        current_combo = combo_head;
      }else{
        current_combo->next = new_cl;
        current_combo = current_combo->next;
      }
    }
    current_list->list = combo_head;
    if(i < net_list->count - 1){ // If it is not the last combination in the list
      current_list->next = calloc(1, sizeof(struct CombinationNode));
      current_list = current_list->next;
    }
  }
  return list_head;
}

// Function that deserializes the NetPile structure received from
// the network to a CardNode structure
struct CardNode *deserialize_pile(struct NetPile *net_pile){
  struct CardNode *pile = NULL;
  for(int i = 0; i < net_pile->count; i++){
    struct Card *card = calloc(1, (sizeof(struct Card)));
    *card = net_pile->cards[i];
    if(pile == NULL){
      pile = append_card(pile, card);
    }else{
      append_card(pile, card);
    }
  }
  return pile;
}