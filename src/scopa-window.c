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

struct _ScopaWindow
{
	AdwApplicationWindow  parent_instance;

	/* Template widgets */
	GtkLabel            *label;
};

G_DEFINE_FINAL_TYPE (ScopaWindow, scopa_window, ADW_TYPE_APPLICATION_WINDOW)

static void
scopa_window_class_init (ScopaWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Example/scopa-window.ui");
	gtk_widget_class_bind_template_child (widget_class, ScopaWindow, label);
}

static void
scopa_window_init (ScopaWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
}
