#pragma once

#include <adwaita.h>
#include "engine/game-assets.h"

#define SELECT_COMBINATION_TYPE_WINDOW (select_combination_window_get_type())

G_DECLARE_FINAL_TYPE (EndGameDialogWindow, endgame_dialog_window,
                       ENDGAMEDIALOG, WINDOW, AdwApplicationWindow)

struct _EndGameDialogWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
    GtkLabel        *player1_card_count;
    GtkLabel        *player1_ori_count;
    GtkLabel        *player1_scope_count;
    GtkLabel        *player1_settebello;
    GtkLabel        *player2_card_count;
    GtkLabel        *player2_ori_count;
    GtkLabel        *player2_scope_count;
    GtkLabel        *player2_settebello;
    GtkListView     *player1_listview;
    GtkListView     *player2_listview;
};

void set_scope(EndGameDialogWindow *self, int player1_count, int player2_count);
void set_card_count(EndGameDialogWindow *self, int player1_count, int player2_count);
void set_ori_count(EndGameDialogWindow *self, int player1_count, int player2_count);
void set_settebello(EndGameDialogWindow *self, GtkLabel *label);

G_END_DECLS
