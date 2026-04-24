#include <stdio.h>
#include <stdlib.h>

#include "netutils/net-assets.h"
#include "netutils/serializer.h"

// Function that serializes the Hand structure to a structure that
// does not use pointers so taht can be send over the network
struct NetHand *serialize_hand(struct Hand * hand){
  struct NetHand *net_hand = malloc(sizeof(struct NetHand));
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
// does not use pointers so taht can be send over the network
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