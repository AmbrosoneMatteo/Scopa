/* new-game.h
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

#include "endgame-dialog.h"

G_DEFINE_FINAL_TYPE (EndGameDialogWindow, endgame_dialog_window,
                     ADW_TYPE_APPLICATION_WINDOW)

static void
endgame_dialog_window_class_init (EndGameDialogWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                          "/org/gnome/Example/endgame-dialog.ui");

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_card_count);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_card_count);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_ori_count);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_ori_count);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_scope_count);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_scope_count);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_settebello);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_settebello);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_listview);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_listview);
}

void set_scope(EndGameDialogWindow *self, int player1_count, int player2_count) {
    char *s1;
    char *s2;
    asprintf(&s1, "%d", player1_count);
    asprintf(&s2, "%d", player2_count);
    gtk_label_set_text (self->player1_scope_count, s1);
    gtk_label_set_text (self->player2_scope_count, s2);
}

void set_card_count(EndGameDialogWindow *self, int player1_count, int player2_count) {
    char *s1;
    char *s2;
    asprintf(&s1, "%d", player1_count);
    asprintf(&s2, "%d", player2_count);
    gtk_label_set_text (self->player1_card_count, s1);
    gtk_label_set_text (self->player2_card_count, s2);
}

void set_ori_count(EndGameDialogWindow *self, int player1_count, int player2_count) {
    char *s1;
    char *s2;
    asprintf(&s1, "%d", player1_count);
    asprintf(&s2, "%d", player2_count);
    gtk_label_set_text (self->player1_ori_count, s1);
    gtk_label_set_text (self->player2_ori_count, s2);
}

void set_settebello(EndGameDialogWindow *self, GtkLabel *label) {
    gtk_label_set_text (label, "ül g'hà");
    if(label == self->player1_settebello)
        gtk_label_set_text (self->player2_settebello, "ül g'hà mia");
    else
        gtk_label_set_text (self->player1_settebello, "ül g'hà mia");
}

static void
endgame_dialog_window_init (EndGameDialogWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
}
