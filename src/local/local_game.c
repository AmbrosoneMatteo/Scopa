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

void
player_play_card(int player_card_index, int table_card_index) {
      struct CombinationNode * combinations = get_combinations_for_card(
                player_hand->cards[player_card_index], table);
      struct Card * player_card = player_hand->cards[player_card_index];

      remove_all_box_cards(main_window->table_top);
      if(combinations != NULL){
          struct CombinationList *auto_take = determine_auto_take(combinations);
          if(auto_take != NULL) {
            remove_combination_from_table(table, auto_take, &player_pile);
            remove_card_from_hand(player_hand, player_card);
            // Adding the card that the player had in the hand to their pile
            append_card(player_pile, player_card);
          }  else {

          }
      }else{
          // The player cannot take anything from the table
          // Adding the card the table
          if(table->node == NULL){
            table->node = append_card(NULL, player_card);
          }else{
            append_card(table->node, player_card);
          }
          table->count++;
          remove_card_from_hand(player_hand, player_card);
      }
      remove_all_box_cards (main_window->player_cards);
      remove_all_box_cards (main_window->table_top);
      place_cards_on_table (main_window, table);
      place_all_cards_on_hand (main_window, main_window->player_cards,player_hand);
}

void start_local_game(int difficulty) {
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

