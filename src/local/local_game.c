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
#include <stdlib.h>
#include "local_game.h"
#include "bot_engine.h"
#include "engine/game-helper.h"
#include "engine/game-assets.h"
#include "scopa-application.h"
#include "scopa-window.h"

/**
 Linked list of the memorized cards that the algorithm
 can use against the player, the difficulty regulates the
 probability of the algorithm to forget a card
 After each card the whole list is passed through this probability
*/
struct CardNode * memorized_card = NULL;
struct Hand * player_hand = NULL;
struct Hand * bot_hand = NULL;
struct Deck * deck = NULL;
struct Table * table = NULL;
struct CardNode * player_pile = NULL;
struct CardNode * bot_pile = NULL;
int player_scope = 0;
int bot_scope = 0;
struct CardNode * last_grabber = NULL;

void free_combination_linkedlist(struct CombinationNode* node) {
    if(node == NULL)
        return ;
    do {
        if (node->list == NULL)
            continue;
        struct CombinationList *list = node->list;
        do {
            struct CombinationList *tmp = list->next;
            free(list);
            list = tmp;
        } while(list != NULL);
        struct CombinationNode *tmp = node->next;
        free(node);
        node = tmp;
    } while(node != NULL);
}

void
player_play_card(int player_card_index) {
      memorize_cards_from_array (player_hand->cards);
      memorize_cards (table->node);

      disable_player_cards(main_window->player_cards);
      struct CombinationNode * combinations = get_combinations_for_card(
                player_hand->cards[player_card_index], table);
      struct Card * player_card = player_hand->cards[player_card_index];

      if (local_play_card (player_hand, player_card, player_pile, combinations,
                        table,deck, &player_scope))
          last_grabber = player_pile;

      remove_card_from_hand(player_hand, player_card);
      remove_all_box_cards(main_window->table_top);
      remove_all_box_cards (main_window->player_cards);

      struct Card * played_card = decide_move();
      if(played_card!=NULL) {
          g_print("Played card value: %d and suit: %c\n", played_card->value,
              suit_strings[played_card->suit]);
          combinations = get_combinations_for_card(
              played_card, table);
          if (local_play_card (bot_hand, played_card, bot_pile,
                           combinations, table, deck, &bot_scope))
              last_grabber = bot_pile;
          remove_card_from_hand(bot_hand, played_card);

          remove_all_box_cards (main_window->adversary_cards);
          place_cards_on_table (main_window, table);
      } else {
          g_print("Something went terribly wrong\n");
      }

      if (deck->count==0 && bot_hand->count == 0 && player_hand->count == 0) {
          g_print("game ended, showing results\n");
          // end game and show stats
      } else {
          place_all_cards_on_hand(main_window,main_window->adversary_cards,bot_hand);
          place_all_cards_on_hand(main_window,main_window->player_cards,player_hand);
          g_print("re-enabling cards: %d - %d - %d\n", deck->count, bot_hand->count, player_hand->count);
          enable_player_cards (main_window->player_cards);
      }
}

void start_local_game(int diff) {
    difficulty = diff;
    player_hand = malloc(sizeof(struct Hand));
    bot_hand = malloc(sizeof(struct Hand));
    deck = deck_init ();
    shuffle_deck (deck);
    get_hand(deck, player_hand);
    get_hand(deck, bot_hand);
    table = table_init_display (deck);
    place_all_cards_on_hand (main_window, main_window->player_cards, player_hand);
    place_all_cards_on_hand (main_window, main_window->adversary_cards, bot_hand);
}



