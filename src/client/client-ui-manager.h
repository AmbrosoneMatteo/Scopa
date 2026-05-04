#ifndef CLIENT_UI_H
#define CLIENT_UI_H
#include "scopa-application.h"

struct TableUiData{
    ScopaWindow *window;
    struct Table *table;
};

struct HandUiData{
    ScopaWindow *window;
    GtkBox *player_hand_box;
    struct Hand *hand;
    bool is_opponent;
};

struct PileUiData{
    GtkImage *pile;
    struct Card *card;
};

void update_ui_table(struct Table * table);
void update_ui_hand(struct Hand * hand);
void update_ui_opponent_hand_cards_count(int cards_in_hand);
void update_ui_enable_cards(void);
void update_ui_disable_cards(void);
void update_ui_place_on_pile(struct Card *card, bool is_opponent_pile);
void update_ui_show_combinations_dialog(struct CombinationNode *combinations);
gboolean client_place_all_cards_on_hand(gpointer user_data);
gboolean client_place_cards_on_table(gpointer user_data);
gboolean client_disable_player_cards(gpointer user_data);
gboolean client_enable_player_cards(gpointer user_data);
gboolean client_place_card_on_pile(gpointer user_data);
gboolean client_show_combinations_dialog(gpointer user_data);

#endif