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

void update_ui_table(struct Table * table);
gboolean client_place_cards_on_table(gpointer user_data);
void update_ui_hand(struct Hand * hand);
void update_ui_opponent_hand_cards_count(int cards_in_hand);
gboolean client_place_all_cards_on_hand(gpointer user_data);

#endif