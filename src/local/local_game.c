/* main.c
 *
 * Copyright 2026 matteo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "local_game.h"
#include "scopa-window.h"
#include "scopa-application.h"
#include "engine/game-helper.h"
#include "engine/game-assets.h"

struct CardNode {
  struct Card * card;
  struct CardNode * next;
  struct CardNode * previous;
};

void send_player_card(struct Card * card);

/**
 Linked list of the memorized cards that the algorithm
 can use against the player, the difficulty regulates the
 probability of the algorithm to forget a card
 After each card the whole list is passed through this probability
*/
struct CardNode * memorized_card = NULL;
struct Card * player_cards[3];
struct Card * bot_cards[3];

void start_local_game(int difficulty) {
    struct Deck * deck = deck_init ();
    shuffle_deck (deck);
    for (int i = 0; i<6;i++) {
        if (i%2==0) {
            player_cards[i]=draw_card (deck);
            send_player_card (player_cards[i]);

        }else {
            bot_cards[i]=draw_card (deck);
            place_adversary_card(main_window);
        }
    }
}

struct CardNode * get_next_memorized_card(struct CardNode * node) {
    return node->next;
}

struct CardNode * get_previous_memorized_card(struct CardNode * node) {
    return node->next;
}

void append_node (struct Card * card) {
  if (memorized_card == NULL) {
      struct CardNode node = {card, NULL, NULL};
      memorized_card = &node;
  } else {
      struct CardNode * current = NULL;
      while ((current = get_next_memorized_card (current)) != NULL)
          ; // get to the last card in the linked lisk
      struct CardNode node = {card, NULL, current};
      current->next = &node;
  }
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
  //the last case is where the node is the only one in the chain
  free(node);
}

bool has_card (struct Card * player_card[],struct Card * card) {
    for (int i = 0;i<3;i++)
        if (player_card[i]->suit == card->suit &&
            player_card[i]->suit == card->value)
            return true;
    return false;
}

//Random number generator using /dev/random
int get_random_integer(void)
{
    unsigned int randval;
    FILE *f;

    f = fopen("/dev/random", "r");
    fread(&randval, sizeof(randval), 1, f);
    fclose(f);

    return randval%100;
}

void send_player_card(struct Card * card) {
  char *path;
  asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png", card->value,
              suit_strings[card->suit]);
  int i = 0;
  while (player_cards[i]==NULL && i<3)
    i++;

  place_player_card (main_window, path,i);
}

/*
 * Run through the whole linked list and with a probability decided by
 * the difficulty variable remove a node, substitute the previous node
 * next address with the next node address and vice versa
 **/
void rerun_probability (int difficulty) {
    int threshold = difficulty*10;
    struct CardNode * index = memorized_card;
    while (index->next != NULL) {
        if (!has_card(bot_cards, index->card)) {
            if (threshold-get_random_integer()<=0) {
                struct CardNode * next = index->next;
                remove_node (index);
                index = next;
            }
        }

    }
}
