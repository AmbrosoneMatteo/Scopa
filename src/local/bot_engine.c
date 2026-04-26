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

int get_random_integer(void);
void rerun_probability (int difficulty);
bool is_not_known(struct Card * card);
bool can_play_this_combination(int sum);
struct Card * decide_move(void);
struct Card * get_card(int value, int suit);
bool table_has_seven_ori(void);

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
            if (!hand_has_card(bot_hand, index->card)) {
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

// This simple function checks if the bot is able to pick the cards in the
// combination with one of his
bool can_play_this_combination(int sum) {
    for (int i = 0; i<HAND_SIZE; i++) {
        if(bot_hand->cards[i]->value == sum)
            return true;
    }
    return false;
}

bool table_has_seven_ori(void) {
    struct CardNode * current = table->node;
    while (current->next!=NULL) {
        if (current->card->value == 7 && current->card->suit == DIAMONDS)
            return true;
        else
            current = current->next;
    }
    return false;
}

struct Card * get_card(int value, int suit) {
    for(int i = 0; i<HAND_SIZE && bot_hand->cards[i]!=NULL; i++) {
        struct Card * card = bot_hand->cards[i];
        if (card->value == value && card->suit == suit)
            return card;
    }
    return NULL;
}

/*
 * This function uses an array list to calculate the possible cards in
 * the player's hand, and with that it determines the best card to play
 **/
struct Card * decide_move(void) {
    struct Card * not_placed_cards[DECK_SIZE];
    struct Card * preferred_card = NULL;
    float card_values_probability[10];
    int card_count = 0;

    for (int i = 0; i<DECK_SIZE; i++) {
        if (is_not_known(&deck->cards[i])) {
            not_placed_cards[card_count++] = &deck->cards[i];
            card_values_probability[deck->cards[i].value]++;
        }
    }
    // this variable holds the probability of a single card that
    // has not come out yet, to be played
    float card_probability = 1/card_count;

    // the specific value with the least probability to come out
    float minimum_probability_value = 1;
    int least_probable_value = 0;

    // this for cycles calculates the probability of a specific value
    // to come out, based on the knowledge of the cards that haven't come out
    for (int i = 0; i<10; i++) {
        card_values_probability[i] = 1/card_values_probability[i];
        if (card_values_probability[i]<minimum_probability_value) {
            minimum_probability_value = card_values_probability[i];
            least_probable_value = i;
        }
    }

    // if the table is NULL, it means that a scopa happened, that means the best
    // card is the one, which is the least probable to come out
    if(table->node == NULL) {
        for (int i = 0; i < DECK_SIZE && bot_hand->cards[i] != NULL; i++) {
            if (bot_hand->cards[i]->value == least_probable_value) {
                // the card with the least probability to cause another scopa
                return bot_hand->cards[i];
            }
        }


        minimum_probability_value = 1; // maximize the probability

        // If the code reaches this point, it means that the card with the value
        // that has the least of probability is not in the bot's hand. So it needs
        // to determine what card in his hand is the least probable to come out
        for(int i = 0; i < DECK_SIZE && bot_hand->cards[i] != NULL; i++) {
            if (card_values_probability[bot_hand->cards[i]->value]
                <minimum_probability_value) {
                minimum_probability_value = card_values_probability[
                                                bot_hand->cards[i]->value];
                preferred_card = bot_hand->cards[i];
            }
        }
        return preferred_card;
    }

    // this variable holds a pointer to a temporary preferred
    // card in the hand, while the algorithm tries to determine
    // a better choice
    //struct CardNode * preferred_card;
    int table_combinations[20][3];
    int count = 0;
    int sum = 0;

    for(int i=0; i<table->count; i++) {
        // if a card on the table, value is equal to the value of one of
        // the card in the bot hand it is added to the array of possible
        // combinations
        if (can_play_this_combination(
              get_node_at_index (table->node, i)->card->value)) {
              table_combinations[count][0] = i;
              table_combinations[count][2] = get_node_at_index
                                      (table->node, i)->card->value;
        }

        for(int l=0; l<table->count; l++) {
            sum = get_node_at_index (table->node, i)->card->value +
                  get_node_at_index (table->node, l)->card->value;

            // if the sum of the value of two cards is more than 10, it cannot
            // legally be taken from the table, as the maximum value of a
            // card is 10 (which is the king)
            if(sum<=10 && can_play_this_combination(sum)) {
                table_combinations[count][0] = i;
                table_combinations[count][1] = l;
                table_combinations[count][2] = sum;
                count++;
            }
        }
    }

    if (table_has_seven_ori() && (preferred_card = get_card (7, DIAMONDS)) != NULL) {
        return preferred_card;
    }

    return NULL;
}
