/* scopa-window.c
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

#include "config.h"
#include <gtk/gtk.h>

#include "scopa-window.h"
#include "engine/game-helper.h"
#include "client/client.h"
#include "local/local_game.h" // TO remove


G_DEFINE_FINAL_TYPE (ScopaWindow, scopa_window, ADW_TYPE_APPLICATION_WINDOW)

int top_cards = 0;
int bottom_cards = 0;

void
remove_all_box_cards(GtkBox* box) {
    GtkWidget *child;
    while((child = gtk_widget_get_first_child ((GtkWidget *)box))
                                                !=NULL) {
        gtk_box_remove (box, child);
    }
}

void
clear_table_cards(ScopaWindow *self) {
    top_cards = 0;
    bottom_cards = 0;
    remove_all_box_cards (self->table_top);
    remove_all_box_cards (self->table_bottom);
}

void
connect_on_drag_begin(GtkDragSource* self,
                      GdkDrag* drag,
                      gpointer user_data) {
    gtk_widget_set_visible ((GtkWidget *)(user_data), false);
    GdkPaintable *paintable = gtk_image_get_paintable((GtkImage *)(user_data));

    gtk_drag_icon_set_from_paintable (drag, paintable, 60, 100);
}

void connect_on_drag_cancel(GtkDragSource* self,
                      GdkDrag* drag,
                      GdkDragCancelReason* reason,
                      gpointer user_data) {
    gtk_widget_set_visible((GtkWidget *)(user_data), true);
}

void
place_all_cards_on_hand(ScopaWindow *window, GtkBox *box, struct Hand* hand) {
    for(int i = 0; i < 3; i++) {
        if(hand->cards[i]!=NULL) {
            char *path;
            if(box==window->player_cards) {
              struct Card *card = hand->cards[i];
              int value = card->value;
              char suit = suit_strings[card->suit];
              asprintf(&path, "/io/github/AmbrosoneMatteo/Scopa/images/DalNegro_Cards/%d_%c.png",
                       value, suit);
            } else {
              path = "/io/github/AmbrosoneMatteo/Scopa/images/retro.svg";
            }
            GtkWidget *image = gtk_image_new_from_resource (path);
            gtk_widget_set_vexpand (image, true);
            gtk_widget_set_hexpand (image, true);
            gtk_widget_set_vexpand_set (image, true);
            gtk_widget_set_hexpand_set (image, true);
            gtk_image_set_pixel_size ((GtkImage*)image, 160);

            if(box==window->player_cards) {
                GtkDragSource *src = gtk_drag_source_new ();
                GdkContentProvider *content = gdk_content_provider_new_typed
                                                            (G_TYPE_INT, i);
                gtk_drag_source_set_content (src, content);
                g_object_unref (content);
                gtk_widget_add_controller (GTK_WIDGET (image),
                                               GTK_EVENT_CONTROLLER (src));
                g_print("%p\n", image);
                g_signal_connect (src, "drag-begin",
                                  G_CALLBACK(connect_on_drag_begin), image);
                g_signal_connect(src, "drag-cancel",
                                 G_CALLBACK(connect_on_drag_cancel), image);
            }
            gtk_box_append (box, image);
        }
    }
}

void
place_cards_on_table(ScopaWindow* window, struct Table* current_table) {
    struct CardNode * node = current_table->node;
    if(node==NULL)
        return ;// No cards present

    do {
        place_card_on_table (window, node->card);
        node = node->next;
    } while (node!=NULL);
}

gboolean
close_request(GtkWindow* self, gpointer user_data) {
    return false;
}

static void
scopa_window_class_init (ScopaWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/io/github/AmbrosoneMatteo/Scopa/scopa-window.ui");
	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, stack_card_image);
  	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, player_cards);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, player1_pile);
    gtk_widget_class_bind_template_child (widget_class, ScopaWindow, player2_pile);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, adversary_cards);
    gtk_widget_class_bind_template_child (widget_class, ScopaWindow, table);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, table_top);
    gtk_widget_class_bind_template_child (widget_class, ScopaWindow, table_bottom);
  	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, pile_box);
}

static gboolean play_card_idle(gpointer user_data) {
    int index = GPOINTER_TO_INT(user_data);
    player_play_card(index);
    return G_SOURCE_REMOVE;
}

static gboolean
place_card (GtkDropTarget* self, const GValue* ptr, gdouble x, gdouble y, gpointer user_data) {
    int player_card_index = g_value_get_int(ptr);
    // If the game is over the network the dropped card must be sent
    // to the client thread through the queue
    if(is_network_game){
        // Allocating memory for the integer index because passing it
        // by GINT_TO_POINTER will be NULL if the integer is equal to 0
        int *index = malloc(sizeof(int));
        *index = player_card_index;
        // Sending card into the queue
        g_async_queue_push (player_card_queue, index);
    }else{
        g_idle_add(play_card_idle, GINT_TO_POINTER(player_card_index));
    }
    return TRUE;
}

void
place_card_on_pile(GtkImage* pile, struct Card* card) {
    if(card!=NULL) {
        char *path;
        int value = card->value;
        char suit = suit_strings[card->suit];
        asprintf(&path, "/io/github/AmbrosoneMatteo/Scopa/images/DalNegro_Cards/%d_%c.png",
                 value, suit);
        gtk_image_set_from_resource (pile, path);
    }
}

void disable_player_cards(GtkBox *box) {
    gtk_widget_set_sensitive ((GtkWidget *) box, false);
}

void enable_player_cards(GtkBox *box) {
    gtk_widget_set_sensitive ((GtkWidget *) box, true);
}

void place_card_on_table(ScopaWindow *window, struct Card * card) {
    char *path;
    int value = card->value;
    char suit = suit_strings[card->suit];
    asprintf(&path, "/io/github/AmbrosoneMatteo/Scopa/images/DalNegro_Cards/%d_%c.png",
             value, suit);

    GtkWidget *image = gtk_image_new_from_resource (path);
    gtk_widget_set_vexpand (image, true);
    gtk_widget_set_hexpand (image, true);
    gtk_widget_set_vexpand_set (image, true);
    gtk_widget_set_hexpand_set (image, true);
    gtk_image_set_pixel_size ((GtkImage*)image, 160);

    if (top_cards > MAX_TABLE_SIZE) {
        gtk_box_append (window->table_bottom, image);
        bottom_cards++;
    } else {
        gtk_box_append (window->table_top, image);
        top_cards++;
    }
}

static void
scopa_window_init (ScopaWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));

    GtkCssProvider *css = gtk_css_provider_new();
    GError *err = NULL;
    gtk_css_provider_load_from_resource(css,"/io/github/AmbrosoneMatteo/Scopa/window.css");
    if (err) { g_printerr("CSS load error: %s\n", err->message); g_clear_error(&err); }
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                                             GTK_STYLE_PROVIDER(css),
                                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);
    gtk_widget_set_name(GTK_WIDGET(self->table), "table_top");
    gtk_widget_set_name(GTK_WIDGET(self->pile_box), "table_top");
    gtk_widget_set_name(GTK_WIDGET(self->player_cards), "hand");
    gtk_widget_set_name(GTK_WIDGET(self->adversary_cards), "hand");
    gtk_widget_set_name(GTK_WIDGET(self), "window");
    const char *classes[] = {"empty-pile", NULL};
    gtk_widget_set_css_classes (GTK_WIDGET (self->player1_pile), classes);
    gtk_widget_set_css_classes (GTK_WIDGET (self->player2_pile), classes);

    GtkDropTarget *tgt = gtk_drop_target_new (G_TYPE_INT, GDK_ACTION_COPY);
    g_signal_connect (tgt, "drop", G_CALLBACK (place_card), self->table);
    // The ownership of tgt is taken by the instance.
    gtk_widget_add_controller (GTK_WIDGET (self->table), GTK_EVENT_CONTROLLER (tgt));
}






