/* new-game.c
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
#include "new-game.h"

enum {
    SIGNAL_START_LOCAL,
    SIGNAL_START_NETWORK,
    SIGNAL_START_SERVER,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

struct _NewGameWindow
{
    AdwApplicationWindow  parent_instance;
    GtkSpinButton        *difficulty_input;
};

static void on_start_local_clicked (GtkButton *button, NewGameWindow *self);
static void on_start_network_clicked (GtkButton *button, NewGameWindow *self);
static void on_start_server_clicked (GtkButton *button, NewGameWindow *self);
G_DEFINE_FINAL_TYPE (NewGameWindow, new_game_window, ADW_TYPE_APPLICATION_WINDOW)

static void
new_game_window_class_init (NewGameWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Example/new-game.ui");
    gtk_widget_class_bind_template_child (widget_class, NewGameWindow, difficulty_input);

    gtk_widget_class_bind_template_callback (widget_class, on_start_local_clicked);
    gtk_widget_class_bind_template_callback (widget_class, on_start_network_clicked);
    gtk_widget_class_bind_template_callback (widget_class, on_start_server_clicked);

    signals[SIGNAL_START_LOCAL] = g_signal_new ("start_game",
                                             G_TYPE_FROM_CLASS (klass),
                                             G_SIGNAL_RUN_LAST,
                                             0, NULL, NULL, NULL,
                                             G_TYPE_NONE, 0);
}

static void
on_start_local_clicked (GtkButton *button, NewGameWindow *self)
{
    g_signal_emit (self, signals[SIGNAL_START_LOCAL], 0);
}

static void
new_game_window_init (NewGameWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));
}

static void
on_start_network_clicked (GtkButton *button, NewGameWindow *self)
{
    g_signal_emit (self, signals[SIGNAL_START_NETWORK], 0);
}

static void
on_start_server_clicked (GtkButton *button, NewGameWindow *self)
{
    g_signal_emit (self, signals[SIGNAL_START_NETWORK], 0);
}

int
new_game_window_get_difficulty (NewGameWindow *self)
{
    return gtk_spin_button_get_value_as_int (self->difficulty_input);
}
