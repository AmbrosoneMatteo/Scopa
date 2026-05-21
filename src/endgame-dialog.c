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
#include "scopa-application.h"

G_DEFINE_FINAL_TYPE (EndGameDialogWindow, endgame_dialog_window,
                     ADW_TYPE_APPLICATION_WINDOW)

#define CARD_TYPE_ITEM (card_item_get_type())
G_DECLARE_FINAL_TYPE(CardItem, card_item, CARD, ITEM, GObject)

struct _CardItem {
    GObject parent;
    guint   index;
    struct Card *card;
};

G_DEFINE_TYPE(CardItem, card_item, G_TYPE_OBJECT)

static void card_item_class_init(CardItemClass *klass) { (void)klass; }
static void card_item_init(CardItem *self)              { (void)self;  }
static void on_start_new_game_clicked (GSimpleAction *action,
                               GVariant      *parameter,
                               gpointer       user_data);

static CardItem *
card_item_new(guint index, struct Card *card)
{
    CardItem *item = g_object_new(CARD_TYPE_ITEM, NULL);
    item->index = index;
    item->card  = card;
    return item;
}

static void
endgame_dialog_window_class_init (EndGameDialogWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                          "/io/github/AmbrosoneMatteo/Scopa/endgame-dialog.ui");

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
                                          player1_sette_count);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_sette_count);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_settebello);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_settebello);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_listview);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_listview);

    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player1_win);
    gtk_widget_class_bind_template_child (widget_class, EndGameDialogWindow,
                                          player2_win);
}

static void
on_start_new_game_clicked (GSimpleAction *action,
                               GVariant      *parameter,
                               gpointer       user_data) {

}

void
build_card_item(GtkWidget *image, struct Card *card)
{

    char *path;
    int value = card->value;
    char suit = suit_strings[card->suit];
    asprintf(&path, "/io/github/AmbrosoneMatteo/Scopa/images/DalNegro_Cards/%d_%c.png",
             value, suit);

    gtk_image_set_from_resource ((GtkImage *)image, path);

    gtk_widget_set_vexpand (image, true);
    gtk_widget_set_hexpand (image, true);
    gtk_widget_set_vexpand_set (image, true);
    gtk_widget_set_hexpand_set (image, true);
    gtk_image_set_pixel_size ((GtkImage*)image, 160);
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item)
{
  GtkWidget *image = gtk_image_new();
  gtk_list_item_set_child (list_item, image);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item,
                  gpointer           *user_data)
{
  (void)factory;
  GtkWidget *image;
  CardItem *item;

  image = gtk_list_item_get_child (list_item);
  item = CARD_ITEM(gtk_list_item_get_item(list_item));
  GtkWidget *list_view = gtk_list_item_get_child(list_item);

  build_card_item(image, item->card);
}

void set_cards(EndGameDialogWindow *self,
               struct CardNode     *player1_pile,
               struct CardNode     *player2_pile,
               int player1_scope,
               int player2_scope) {
    set_scope(self, player1_scope, player2_scope);
    int count = 0;
    int ori = 0;
    int sette = 0;
    bool settebello = false;

    int player1_points = 0;
    int player2_points = 0;

    self->store1 = g_list_store_new(CARD_TYPE_ITEM);
    self->store2 = g_list_store_new(CARD_TYPE_ITEM);
    // assign the scores for the player 1. For the second is sufficient to invert
    // the scores
    if (player1_pile!=NULL) {
        do {
            count++;
            struct Card * card = player1_pile->card;
            if (card->suit==DIAMONDS) {
                ori++;
                if (card->value == 7){
                    sette++;
                    set_settebello (self, self->player1_settebello);
                    player1_points++;
                    settebello=true;
                }
            } else if (card->value==7 && card->suit!=DIAMONDS)
                sette++;

            // create the item for the store
            CardItem *item = card_item_new (count, card);
            g_list_store_append(self->store1, item);
            g_object_unref (item);

            player1_pile = player1_pile->next;
        } while(player1_pile!=NULL);
    }

    // create and assign the items for the second player
    if (player2_pile!=NULL) {
        do {
            struct Card * card = player2_pile->card;
            CardItem *item = card_item_new (count, card);
            g_list_store_append(self->store2, item);
            g_object_unref (item);
            player2_pile = player2_pile->next;
        } while(player2_pile!=NULL);
    }

    GtkNoSelection *selection1 = gtk_no_selection_new(G_LIST_MODEL(self->store1));
    GtkListItemFactory *factory1 = gtk_signal_list_item_factory_new ();
    g_signal_connect (factory1, "setup", G_CALLBACK (setup_listitem_cb), NULL);
    g_signal_connect (factory1, "bind", G_CALLBACK(bind_listitem_cb), self);
    gtk_list_view_set_factory(self->player1_listview, factory1);
    gtk_list_view_set_model(self->player1_listview, GTK_SELECTION_MODEL(selection1));

    GtkNoSelection *selection2 = gtk_no_selection_new(G_LIST_MODEL(self->store2));
    GtkListItemFactory *factory2 = gtk_signal_list_item_factory_new ();
    g_signal_connect (factory2, "setup", G_CALLBACK (setup_listitem_cb), NULL);
    g_signal_connect (factory2, "bind", G_CALLBACK(bind_listitem_cb), self);
    gtk_list_view_set_factory(self->player2_listview, factory2);
    gtk_list_view_set_model(self->player2_listview, GTK_SELECTION_MODEL(selection2));

    g_object_unref(selection1);
    g_object_unref(selection2);
    g_object_unref(factory1);
    g_object_unref(factory2);

    if(!settebello) {
        set_settebello (self, self->player2_settebello);
        player2_points++;
    }

    if(count>20)
        player1_points++;
    else if (count<20)
        player2_points++;

    if(ori>5)
        player1_points++;
    else if(ori<5)
        player2_points++;

    if(sette>2)
        player1_points++;
    else if(sette<2)
        player2_points++;

    player1_points+=player1_scope;
    player2_points+=player2_scope;

    char* msg1;
    char* msg2;
    if(player1_points>player2_points) {
        asprintf (&msg1, "Opponent won points: %d", player1_points);
        asprintf (&msg2, "You lost, points: %d", player2_points);
    }
    else if (player1_points<player2_points) {
        asprintf (&msg1, "Opponent lost, points: %d", player1_points);
        asprintf (&msg2, "You won, points: %d", player2_points);
    } else {
        asprintf (&msg1, "It's a draw, points: %d", player1_points);
        asprintf (&msg2, "It's a draw, points: %d", player2_points);
    }

    gtk_label_set_text (self->player1_win, msg1);
    gtk_label_set_text (self->player2_win, msg2);

    set_card_count(self, count, DECK_SIZE-count);
    set_ori_count (self, ori, 10-ori);
    set_sette_count(self, sette, 4-sette);

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

void set_sette_count(EndGameDialogWindow *self, int player1_count, int player2_count) {
    char *s1;
    char *s2;
    asprintf(&s1, "%d", player1_count);
    asprintf(&s2, "%d", player2_count);
    gtk_label_set_text (self->player1_sette_count, s1);
    gtk_label_set_text (self->player2_sette_count, s2);
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
    gtk_label_set_text (label, "ül g'hà"); // (The player has the settebello)
    if(label == self->player1_settebello)
        gtk_label_set_text (self->player2_settebello, "ül g'hà mia"); // (The player does not have the settebello)
    else
        gtk_label_set_text (self->player1_settebello, "ül g'hà mia");
}

static const GActionEntry dialog_actions[] = {
	{ "new_game_button_clicked", on_start_new_game_clicked },
};

static void
endgame_dialog_window_init (EndGameDialogWindow *self)
{
  	g_action_map_add_action_entries (G_ACTION_MAP (self),
	                                 dialog_actions,
	                                 G_N_ELEMENTS (dialog_actions),
	                                 self);
    gtk_widget_init_template (GTK_WIDGET (self));
}



