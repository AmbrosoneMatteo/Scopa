/* scopa-window.h
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

#pragma once

#include <adwaita.h>
#include "engine/game-assets.h"

G_BEGIN_DECLS

#define SCOPA_TYPE_WINDOW (scopa_window_get_type())

G_DECLARE_FINAL_TYPE (ScopaWindow, scopa_window, SCOPA, WINDOW, AdwApplicationWindow)
void place_player_card (ScopaWindow *window, struct Card * card, int index);
void place_card_on_table(ScopaWindow *window, struct Card * card);
void remove_all_box_cards(GtkBox* box);
void place_all_cards_on_hand(ScopaWindow *window,
                             GtkBox      *box,
                             struct Hand *hand);
void place_cards_on_table(ScopaWindow* window, struct Table* current_table);
void disable_player_cards(GtkBox *box);
void enable_player_cards(GtkBox *box);
void place_card_on_pile(GtkImage* pile, struct Card* card);
void clear_table_cards(ScopaWindow *self);
struct _ScopaWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
        GtkImage        *stack_card_image;
        GtkBox          *player_cards;
        GtkBox          *adversary_cards;
        GtkBox          *table_top;
        GtkBox          *table_bottom;
        GtkBox          *pile_box;
        GtkBox          *table;
        GtkImage        *player1_pile;
        GtkImage        *player2_pile;
};

G_END_DECLS

