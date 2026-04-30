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

#pragma once

#include <adwaita.h>
#include "engine/game-assets.h"

#define SELECT_COMBINATION_TYPE_WINDOW (select_combination_window_get_type())

struct _SelectCombinationWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
    GtkListView        *list_view;
};

G_DECLARE_FINAL_TYPE (SelectCombinationWindow, select_combination_window,
                       NEW, WINDOW, AdwApplicationWindow)

int select_combination_get_index(void);
void add_combinations(SelectCombinationWindow *self,
                      struct CombinationNode  *node);


G_END_DECLS
