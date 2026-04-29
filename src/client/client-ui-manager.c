#include "engine/game-assets.h"
#include "client-ui-manager.h"

// Function that passes the data to the UI function that
// updates the table on the GUI. The function is called
// with g_idle_add that allowes the safe update on the GUI thread
void update_ui_table(struct Table * table){
    if(table == NULL){
        return;
    }
    struct TableUiData *data = g_new(struct TableUiData, 1);
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
    struct HandUiData *data = g_new(struct HandUiData, 1);
    data->window = main_window;
    data->hand = hand;
    g_idle_add(client_place_all_cards_on_hand, data);
}

gboolean client_place_cards_on_table(gpointer user_data){
    struct TableUiData *data = (struct TableUiData*)user_data;
    // Calling function from scopa-window
    place_cards_on_table(data->window, data->table);
    g_free(data);

    return G_SOURCE_REMOVE; // Run the function only once
}

gboolean client_place_all_cards_on_hand(gpointer user_data){
    struct HandUiData *data = (struct HandUiData*)user_data;
    place_all_cards_on_hand(data->window, data->window->player_cards, data->hand);
    g_free(data);

    return G_SOURCE_REMOVE;
}