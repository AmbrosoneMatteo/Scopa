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

void start_local_game(int difficulty) {
    deck = deck_init ();
    shuffle_deck (deck);
    player_hand = get_hand(deck);
    bot_hand = get_hand(deck);
    table = table_init (deck);
    for(int i = 0; i<HAND_SIZE; i++) {
        send_player_card (player_hand->cards[i], i);
    }
    for(int i = 0; i < 3; i++) {
        place_adversary_card (main_window);
    }

   // test code snippet
    struct CombinationNode * possibilities = calculate_possible_combination(
                                                  player_hand, table);
    if(possibilities!=NULL) {
        print_list(possibilities);
    } else {
        g_print("No combination available");
    }
}

bool has_card (struct Hand * hand,struct Card * card) {
    for (int i = 0;i<3;i++)
        if (hand->cards[i]->suit == card->suit &&
            hand->cards[i]->suit == card->value)
            return true;
    return false;
}

