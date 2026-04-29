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
void rerun_probability (void);
bool is_known(struct Card * card);
bool can_play_this_combination(int sum);
struct Card * decide_move(void);
struct Card * get_card(int value, int suit);
bool table_has_seven_ori(void);
int difficulty;

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
void rerun_probability (void) {
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


// Memorizes the cards in the linked list
void memorize_cards_from_array(struct Card **cards) {
    for(int i = 0; i<3; i++) {
        if (cards[i] != NULL && is_known (cards[i])) {
            append_card (memorized_card, cards[i]);
        }
    }
}

// Memorizes the cards in the linked list
void memorize_cards(struct CardNode *node) {
    do {
        if (is_known (node->card)) {
            append_card (memorized_card, node->card);
        }
        node = node->next;
    } while (node!=NULL);
}

/**
 * Checks if a card is not already been played
 * */
bool is_known(struct Card * card) {
    struct CardNode * current = memorized_card;
    if (current==NULL)
        return false;

    do {
        if (card->suit == current->card->suit &&
              card->value == current->card->value)
            return true;
        current = current->next;
    } while (current!= NULL);
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

struct Card * get_least_probable_card(float *probabilities) {
    int minimum_probability_value = 1; // maximize the probability
    struct Card * return_card = NULL;
    struct Card **cards= bot_hand->cards;

    for (int i = 0; i<3; i++) {
        if(cards[i]!=NULL &&
                    probabilities[cards[i]->value]<minimum_probability_value) {
            minimum_probability_value = probabilities[cards[i]->value];
            return_card = cards[i];
        }
    }

    return return_card;
}

/*
 * This function uses an array list to calculate the possible cards in
 * the player's hand, and with that it determines the best card to play
 **/
struct Card * decide_move(void) {
    rerun_probability ();

    struct Card * not_placed_cards[DECK_SIZE];
    struct Card * preferred_card = NULL;
    float card_values_probability[10];
    int card_count = 0;

    for (int i = 0; i<DECK_SIZE; i++) {
        if (!is_known(&deck->cards[i])) {
            not_placed_cards[card_count++] = &deck->cards[i];
            card_values_probability[deck->cards[i].value]++;
        }
    }
    // this variable holds the probability of a single card that
    // has not come out yet, to be played
    float card_probability = 1/card_count;


    // this for cycles calculates the probability of a specific value
    // to come out, based on the knowledge of the cards that haven't come out
    for (int i = 0; i<10; i++) {
        card_values_probability[i] = 1/card_values_probability[i];
    }

    // if the table is NULL, it means that a scopa happened, that means the best
    // card is the one, which is the least probable to come out
    if(table->node == NULL) {
        return get_least_probable_card (card_values_probability);
    }

    struct CombinationNode *combinations[3];
    struct Card **cards = bot_hand->cards;

    // Il sett da ür sovra tutci
    if (table_has_seven_ori() && (preferred_card = get_card (7, DIAMONDS)) != NULL) {
        return preferred_card;
    }

    // Get all the possibilities for the current hand
    for(int i = 0; i<3; i++) {
        if (cards[i]!=NULL) {
            combinations[i] = get_combinations_for_card (cards[i], table);
        }
    }

    for (int i = 0; i<3; i++) {
        if(combinations[i]==NULL)
            continue;
    }

    // No card has been chosen, so place the one that has the least of probability
    // to come out
    return get_least_probable_card(card_values_probability);
}
