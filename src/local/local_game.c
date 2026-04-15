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
    table_init (deck);
    for(int i = 0; i<HAND_SIZE; i++) {
        send_player_card (player_hand->cards[i], i);
    }
    place_adversary_card (main_window);
}

bool has_card (struct Hand * hand,struct Card * card) {
    for (int i = 0;i<3;i++)
        if (hand->cards[i]->suit == card->suit &&
            hand->cards[i]->suit == card->value)
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

/*
 * Run through the whole linked list and with a probability decided by
 * the difficulty variable remove a node, substitute the previous node
 * next address with the next node address and vice versa
 **/
void rerun_probability (int difficulty) {
    int threshold = difficulty*10;
    struct CardNode * index = memorized_card;
    if (index != NULL) {
        while (index->next != NULL) {
            if (!has_card(bot_hand, index->card)) {
                if (threshold - get_random_integer()<=0) {
                    struct CardNode * next = index->next;
                    remove_node (index);
                    index = next;
                }
            }
        }
    }
}

