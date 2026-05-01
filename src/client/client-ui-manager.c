#include "engine/game-assets.h"
#include "client-ui-manager.h"

// Function that passes the data to the UI function that
// updates the table on the GUI. The function is called
// with g_idle_add that allowes the safe update on the GUI thread
void update_ui_table(struct Table * table){
    if(table == NULL){
        return;
    }
    struct TableUiData *data = calloc(1, sizeof(struct TableUiData));
    data->window = main_window;
    data->table = table;

    g_idle_add(client_place_cards_on_table, data);
}

// Function that passes the data to the UI function that
// updates the hand on the GUI. The function is called
// with g_idle_add that allowes the safe update on the GUI thread
void update_ui_hand(struct Hand * hand){
    if(hand == NULL){
        return;
    }
    struct HandUiData *data = calloc(1, sizeof(struct HandUiData));
    data->window = main_window;
    data->player_hand_box = main_window->player_cards;
    data->hand = hand;
    data->is_opponent = false;
    g_idle_add(client_place_all_cards_on_hand, data);
}

// Function that takes as param the number of cards that the opponent
// has in his hand and calls the UI function that updates the hand.
// The hand passed to the update function has dummy data since the opponent
// cards are not relevant and not available on the client
void update_ui_opponent_hand_cards_count(int cards_in_hand){
    struct Hand *dummy_hand = calloc(1, sizeof(struct Hand));
    for(int i = 0; i < cards_in_hand; i++){
        struct Card *dummy_card = calloc(1, sizeof(struct Card));
        dummy_card->suit = 0;
        dummy_card->value = 1;
        dummy_hand->cards[i] = dummy_card;
    }
    dummy_hand->count = cards_in_hand;
    struct HandUiData *data = calloc(1, sizeof(struct HandUiData));
    data->window = main_window;
    data->player_hand_box = main_window->adversary_cards;
    data->hand = dummy_hand;
    data->is_opponent = true;
    g_idle_add(client_place_all_cards_on_hand, data);
}

// Function that updates the UI with the last card played by the player
// or opponent. The card played is displayed in the dedicated pile box.
void update_ui_place_on_pile(struct Card *card, bool is_opponent_pile){
    struct PileUiData *data = calloc(1, sizeof(struct PileUiData));
    data->pile = (is_opponent_pile) ? main_window->player2_pile : main_window->player1_pile;
    data->card = card;
    g_idle_add(client_place_card_on_pile, data);
}

void update_ui_enable_cards(void){
    g_idle_add(client_enable_player_cards, main_window->player_cards);
}

void update_ui_disable_cards(void){
    g_idle_add(client_disable_player_cards, main_window->player_cards);
}

// Function called through g_idle_add that updates the table
gboolean client_place_cards_on_table(gpointer user_data){
    struct TableUiData *data = (struct TableUiData*)user_data;
    // Calling function from scopa-window
    remove_all_box_cards(data->window->table_top);
    place_cards_on_table(data->window, data->table);
    free(data);

    return G_SOURCE_REMOVE; // Run the function only once
}

gboolean client_place_all_cards_on_hand(gpointer user_data){
    struct HandUiData *data = (struct HandUiData*)user_data;
    remove_all_box_cards(data->player_hand_box);
    place_all_cards_on_hand(data->window, data->player_hand_box, data->hand);
    if(data->is_opponent){
        for(int i = 0; i < data->hand->count; i++){
            free(data->hand->cards[i]);
        }
        free(data->hand);
    }
    free(data);

    return G_SOURCE_REMOVE;
}

gboolean client_place_card_on_pile(gpointer user_data){
    struct PileUiData *data = (struct PileUiData*)user_data;
    place_card_on_pile(data->pile, data->card);
    free(data->card);
    free(data);

    return G_SOURCE_REMOVE;
}

gboolean client_enable_player_cards(gpointer user_data){
    enable_player_cards((GtkBox *) user_data);
    return G_SOURCE_REMOVE;
}

gboolean client_disable_player_cards(gpointer user_data){
    disable_player_cards((GtkBox *) user_data);
    return G_SOURCE_REMOVE;
}