#include "engine/game-assets.h"
#include "select-combination.h"
#include "endgame-dialog.h"
#include "client.h"

#include "client-ui-manager.h"

// Function that passes the data to the UI function that
// updates the table on the GUI. The function is called
// with g_idle_add that allows the safe update on the GUI thread
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
// with g_idle_add that allows the safe update on the GUI thread
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

// Function that shows the dialog for the user to select an available
// combination to play if there are more than 2
void update_ui_show_combinations_dialog(struct CombinationNode *combinations){
    g_idle_add(client_show_combinations_dialog, combinations);
}

// Function that shows the endgame dialog with all the points and cards
// that were played during the game
void update_ui_endgame_dialog(struct CardNode *player_pile,
    struct CardNode *opponent_pile, int player_scope, int opponent_scope){

    struct EndGameUiData *data = calloc(1, sizeof(struct EndGameUiData));
    data->player_pile = player_pile;
    data->opponent_pile = opponent_pile;
    data->player_scope = player_scope;
    data->opponent_scope = opponent_scope;
    g_idle_add(client_show_endgame_dialog, data);
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
    clear_table_cards(data->window);
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

gboolean client_show_combinations_dialog(gpointer user_data){
    struct CombinationNode *combinations = (struct CombinationNode *)user_data;
    ScopaApplication *app = SCOPA_APPLICATION(g_application_get_default());
    GtkWindow *parent = gtk_application_get_active_window(GTK_APPLICATION(app));
    SelectCombinationWindow *window = g_object_new(
        SELECT_COMBINATION_TYPE_WINDOW,
        "application", app,
        "transient-for", parent,
        "modal", TRUE,
        NULL
    );
    g_signal_connect(window, "destroy", G_CALLBACK(on_network_combo_window_destroyed), NULL);

    add_combinations(window, combinations);
    gtk_window_present(GTK_WINDOW(window));

    return G_SOURCE_REMOVE;
}

static void on_network_combo_window_destroyed(GtkWidget *window, gpointer user_data){
    int *combo_index = malloc(sizeof(int));
    *combo_index = 0; // Select first available combination
    g_async_queue_push(player_combo_queue, combo_index);
}

gboolean client_show_endgame_dialog(gpointer user_data){
    struct EndGameUiData *data = (struct EndGameUiData*)user_data;
    ScopaApplication *app = SCOPA_APPLICATION(g_application_get_default());
    GtkWindow *parent = gtk_application_get_active_window(GTK_APPLICATION(app));
    EndGameDialogWindow *window = g_object_new(
        ENDGAME_DIALOG_TYPE_WINDOW,
        "application", app,
        "transient-for", parent,
        "modal", TRUE,
        NULL
    );
    set_cards(window, data->player_pile, data->opponent_pile, data->player_scope, data->opponent_scope);
    gtk_window_present(GTK_WINDOW(window));
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
