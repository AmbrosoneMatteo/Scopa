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

G_BEGIN_DECLS

#define NEW_GAME_TYPE_WINDOW (new_game_window_get_type())

G_DECLARE_FINAL_TYPE (NewGameWindow, new_game_window, NEW, WINDOW, AdwApplicationWindow)

int new_game_window_get_difficulty (NewGameWindow *self);
int new_game_window_get_server_port(NewGameWindow *self);
int new_game_window_get_connect_port(NewGameWindow *self);
char *new_game_window_get_connect_host(NewGameWindow *self);

G_END_DECLS
