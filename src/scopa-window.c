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

#include "scopa-window.h"
#include "engine/game-helper.h"

struct _ScopaWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
        GtkImage        *stack_card_image;
        GtkBox          *player_cards;
        GtkBox          *adversary_cards;
        GtkBox          *table_top;
        GtkBox          *table_bottom;
};

struct WidgetLinkedList {
    GtkWidget *ptr;
    struct WidgetLinkedList *next;
};

GtkImage *player_cards[3];
GtkImage *adversary_cards[3];
struct WidgetLinkedList *widgets_on_table = NULL;

void append_widget(GtkWidget *widget) {
    struct WidgetLinkedList *current = widgets_on_table;
    struct WidgetLinkedList * new_node = (struct WidgetLinkedList*)
                            malloc(sizeof(struct WidgetLinkedList));
    new_node->ptr = widget;
    if(current == NULL) {
        widgets_on_table = new_node;
    } else {
        do {
            current = current->next;
        } while (current!=NULL);
        current->next = new_node;
    }
}

struct WidgetLinkedList * get_widget_at_index(int index) {
    struct WidgetLinkedList *out = widgets_on_table;
    int count = 0;

    if(out == NULL)
        return NULL;

    while (count < index && out->next!=NULL) {
          out = out->next;
          count++;
    }
    if (count==index)
        return out->next;

    return out;
}

void remove_widget_at_index(ScopaWindow *window, int index) {
    if (widgets_on_table==NULL)
        return ;

    if(index!=0) {

    } else {
        struct WidgetLinkedList *tmp = widgets_on_table;
        if(widgets_on_table->next!=NULL)
            widgets_on_table = widgets_on_table->next;
        else
            widgets_on_table = NULL;
        gtk_box_remove (window->table_top, tmp->ptr);
        g_free(tmp->ptr); //frees widget from memory
        free(tmp);  // frees the node from memory
    }
}

G_DEFINE_FINAL_TYPE (ScopaWindow, scopa_window, ADW_TYPE_APPLICATION_WINDOW)

static void
scopa_window_class_init (ScopaWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Example/scopa-window.ui");
	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, stack_card_image);
  	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, player_cards);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, adversary_cards);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, table_top);
    	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, table_bottom);
}

static gboolean
place_card (GtkDropTarget* self, const GValue* ptr, gdouble x, gdouble y, gpointer user_data) {
    int player_card_index = g_value_get_int(ptr);

    int table_card_index = g_value_get_int (user_data);

    player_play_cards(player_card_index, table_card_index);

    return TRUE;
}

void
remove_card_from_table(ScopaWindow *window, int index) {

}

void remove_card_from_player_hand(ScopaWindow *window, int index) {

}

void remove_card_from_adversary_hand(ScopaWindow* window, int index) {

}

void
place_player_card (ScopaWindow *window, struct Card * card, int index) {
    char *path;
    int value = card->value;
    char suit = suit_strings[card->suit];
    asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png",
             value, suit);
    g_print("passing pointer: %p\n", card);

    g_print("Placing card: %s\n", path);
    GtkWidget *image = gtk_image_new_from_resource (path);
    gtk_widget_set_vexpand (image, true);
    gtk_widget_set_hexpand (image, true);
    gtk_widget_set_vexpand_set (image, true);
    gtk_widget_set_hexpand_set (image, true);
    gtk_image_set_pixel_size ((GtkImage*)image, 160);

    GtkDragSource *src = gtk_drag_source_new ();
    GdkContentProvider *content = gdk_content_provider_new_typed (G_TYPE_INT, index);
    gtk_drag_source_set_content (src, content);
    g_object_unref (content);
    gtk_widget_add_controller (GTK_WIDGET (image), GTK_EVENT_CONTROLLER (src));
    gtk_box_append (window->player_cards, image);
}

void place_adversary_card(ScopaWindow *window) {
    g_print("Placing adversary card\n");
    GtkWidget *image = gtk_image_new_from_resource ("/org/gnome/Example/images/retro.svg");
    gtk_widget_set_vexpand (image, true);
    gtk_widget_set_hexpand (image, true);
    gtk_widget_set_vexpand_set (image, true);
    gtk_widget_set_hexpand_set (image, true);
    gtk_image_set_pixel_size ((GtkImage*)image, 160);
    gtk_box_append (window->adversary_cards, image);
}

void place_card_on_table(ScopaWindow *window, struct Card * card, int index) {
    char *path;
    int value = card->value;
    char suit = suit_strings[card->suit];
    asprintf(&path, "/org/gnome/Example/images/DalNegro_Cards/%d_%c.png",
             value, suit);

    g_print("Placing card: %s\n", path);
    GtkWidget *image = gtk_image_new_from_resource (path);
    gtk_widget_set_vexpand (image, true);
    gtk_widget_set_hexpand (image, true);
    gtk_widget_set_vexpand_set (image, true);
    gtk_widget_set_hexpand_set (image, true);
    gtk_image_set_pixel_size ((GtkImage*)image, 160);

    GtkDropTarget *tgt = gtk_drop_target_new (G_TYPE_INT, GDK_ACTION_COPY);
    g_signal_connect (tgt, "drop", G_CALLBACK (place_card), &index);
    gtk_widget_add_controller (GTK_WIDGET (image), GTK_EVENT_CONTROLLER (tgt)); // The ownership of tgt is taken by the instance.

    gtk_box_append (window->table_top, image);
}

static void
scopa_window_init (ScopaWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
}
