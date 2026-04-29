#ifndef CLIENT_UI_H
#define CLIENT_UI_H
#include "scopa-application.h"

struct TableUiData{
    ScopaWindow *window;
    struct Table *table;
};

struct HandUiData{
    ScopaWindow *window;
    struct Hand *hand;
};

void update_ui_table(struct Table * table);
gboolean client_place_cards_on_table(gpointer user_data);
void update_ui_hand(struct Hand * hand);
gboolean client_place_all_cards_on_hand(gpointer user_data);

#endif