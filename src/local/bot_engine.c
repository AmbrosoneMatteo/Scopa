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
#include <stdio.h>
#include <stdbool.h>
#include "local_game.h"
#include "engine/game-assets.h"
#include "engine/game-helper.h"

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

/**
 * Checks if a card is not already been played
 * */
bool is_not_known(struct Card * card) {
    struct CardNode * current = memorized_card;
    while(current->next != NULL) {
        if (card->suit == current->card->suit &&
              card->value == current->card->value)
            return true;
    }
    return false;
}

/*
 * This function uses an array list to calculate the possible cards in
 * the player's hand, and with that it determines the best card to play
 **/
struct Card * decide_move(void) {
    struct Card * not_placed_cards[DECK_SIZE];
    int card_count = 0;
    int spades_count = 0;
    int clubs_count = 0;
    int diamonds_count = 0;
    int hearts_count = 0;

    for (int i = 0; i<DECK_SIZE; i++) {
        if (is_not_known(&deck->cards[i])) {
            not_placed_cards[card_count++] = &deck->cards[i];
            switch (deck->cards[i].suit) {
                case CLUBS:
                    clubs_count++;
                    break;
                case DIAMONDS:
                    diamonds_count++;
                    break;
                case HEARTS:
                    hearts_count++;
                    break;
                case SPADES:
                    spades_count++;
                    break;
                default: break;
            }
        }
    }
    // this variable holds the probability of a card that
    // has not come out yet, to be played
    float card_probability = 1/card_count;
    float spades_probability = 0/spades_count;
    float clubs_probability = 0/clubs_count;
    float diamonds_probability = 0/diamonds_count;
    float hearts_probability = 0/hearts_count;

    // this variable holds a pointer to a temporary preferred
    // card in the hand, while the algorithm tries to determine
    // a better choice
    struct CardNode * preferred_card;
    int table_combinations[20][3];
    int count = 0;
    int sum = 0;

    for(int i=0; i<table->count; i++) {
        for(int l=0; l<table->count; l++) {
            sum = get_node_at_index (table->node, i)->card->value +
                  get_node_at_index (table->node, l)->card->value;

            // if the sum of the value of two cards is more than 10, it cannot
            // legally be taken from the table, as the maximum value of a
            // card is 10 (which is the king)
            if(sum<=10) {
                table_combinations[count][0] = i;
                table_combinations[count][1] = l;
                table_combinations[count][2] = sum;
                count++;
            }
        }
    }

    int bot_hand_combination[6][3];
    count = 0;
    for(int i = 0; i<2;i++) {
        for(int l = i+1; i<HAND_SIZE; i++) {
            sum = bot_hand->cards[i]->value + bot_hand->cards[l]->value;
            if(sum <= 10) {
                bot_hand_combination[count++][0] = i;
                bot_hand_combination[count++][1] = l;
                bot_hand_combination[count++][2] = sum;
            }
        }
    }
    return NULL;
}
