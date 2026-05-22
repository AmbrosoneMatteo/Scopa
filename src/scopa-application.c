/* scopa-application.c
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
#include <glib/gi18n.h>

#include "scopa-application.h"
#include "new-game.h"
#include "local/local_game.h"
#include "server/server.h"
#include "client/client.h"

ScopaWindow *main_window = NULL;

struct _ScopaApplication
{
	AdwApplication parent_instance;
};
struct ServerArgs {
	int port;
};
struct ClientArgs {
	char *host;
	int port;
};

G_DEFINE_FINAL_TYPE (ScopaApplication, scopa_application, ADW_TYPE_APPLICATION)

static void on_start_local (NewGameWindow *window, gpointer user_data);
static void on_start_network (NewGameWindow *window, gpointer user_data);
static void on_start_server (NewGameWindow *window, gpointer user_data);
static gpointer client_thread_func(gpointer data);
static gpointer server_thread_func(gpointer data);

ScopaApplication *
scopa_application_new (const char        *application_id,
                       GApplicationFlags  flags)
{
	g_return_val_if_fail (application_id != NULL, NULL);

	return g_object_new (SCOPA_TYPE_APPLICATION,
	                     "application-id", application_id,
	                     "flags", flags,
	                     "resource-base-path", "/io/github/AmbrosoneMatteo/Scopa",
	                     NULL);
}

static void
scopa_application_activate (GApplication *app)
{
	GtkWindow *window;

	g_assert (SCOPA_IS_APPLICATION (app));

	window = gtk_application_get_active_window (GTK_APPLICATION (app));

	if (window == NULL) {
		window = g_object_new (SCOPA_TYPE_WINDOW,
		                       "application", app,
		                       NULL);
                main_window = (ScopaWindow*) window;
        }
	gtk_window_present (window);
}

static void
scopa_application_class_init (ScopaApplicationClass *klass)
{
	GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

	app_class->activate = scopa_application_activate;
}

static void
scopa_application_about_action (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       user_data)
{
	static const char *developers[] = {"matteo", NULL};
	ScopaApplication *self = user_data;
	GtkWindow *window = NULL;

	g_assert (SCOPA_IS_APPLICATION (self));

	window = gtk_application_get_active_window (GTK_APPLICATION (self));

	adw_show_about_dialog (GTK_WIDGET (window),
	                       "application-name", "scopa",
	                       "application-icon", "io.github.AmbrosoneMatteo.Scopa",
	                       "developer-name", "matteo",
	                       "translator-credits", _("translator-credits"),
	                       "version", "0.1.0",
	                       "developers", developers,
	                       "copyright", "© 2026 matteo",
	                       NULL);
}

static void
scopa_application_quit_action (GSimpleAction *action,
                               GVariant      *parameter,
                               gpointer       user_data)
{
	ScopaApplication *self = user_data;

	g_assert (SCOPA_IS_APPLICATION (self));

	g_application_quit (G_APPLICATION (self));
}

static void
scopa_application_new_game_action (GSimpleAction *action,
                                   GVariant      *parameter,
                                   gpointer       user_data)
{
    ScopaApplication *self = user_data;
    GtkWindow *parent;
    NewGameWindow *window;

    g_assert (SCOPA_IS_APPLICATION (self));

    parent = gtk_application_get_active_window (GTK_APPLICATION (self));

    window = g_object_new (NEW_GAME_TYPE_WINDOW,
                           "application", self,
                           "transient-for", parent,
                           "modal", TRUE,
                           NULL);

    g_signal_connect (window, "start-local", G_CALLBACK (on_start_local), self);
    g_signal_connect (window, "start-network", G_CALLBACK (on_start_network), self);
    g_signal_connect (window, "start-server", G_CALLBACK (on_start_server), self);

    gtk_window_present (GTK_WINDOW (window));
}

static void
on_start_local (NewGameWindow *window, gpointer user_data)
{
    int diff = new_game_window_get_difficulty (window);
    gtk_window_close (GTK_WINDOW (window));
    g_print("starting local game with difficulty: %d\n", diff);
    start_local_game (diff);
}

static void
on_start_network (NewGameWindow *window, gpointer user_data)
{
    gtk_window_close (GTK_WINDOW (window));
	int connect_port = new_game_window_get_connect_port (window);
	char *connect_host = new_game_window_get_connect_host (window);
	struct ClientArgs *args = g_new(struct ClientArgs, 1);
	args->host = g_strdup(connect_host);
	args->port = connect_port;
	// starting client thread
	g_thread_new("client-thread", client_thread_func, args);
}

static void
on_start_server (NewGameWindow *window, gpointer user_data)
{
    gtk_window_close (GTK_WINDOW (window));
	int srv_port = new_game_window_get_server_port (window);
	struct ServerArgs *srv_args = g_new(struct ServerArgs, 1);
	srv_args->port = srv_port;
	// starting server thread
	g_thread_new("server-thread", server_thread_func, srv_args);

	struct ClientArgs *client_args = g_new(struct ClientArgs, 1);
	client_args->host = g_strdup("127.0.0.1");
	client_args->port = srv_port;
	// Starting the client thread that connects to the local server
	g_thread_new("client-thread", client_thread_func, client_args);
}

static gpointer client_thread_func(gpointer data) {
    struct ClientArgs *args = (struct ClientArgs *)data; 
    start_client(args->host, args->port); 
    g_free(args->host);
    g_free(args); 
    return NULL;
}

static gpointer server_thread_func(gpointer data) {
    struct ServerArgs *args = (struct ServerArgs *)data; 
    start_server(args->port); 
    g_free(args); 
    return NULL;
}

static const GActionEntry app_actions[] = {
	{ "quit", scopa_application_quit_action },
	{ "about", scopa_application_about_action },
        { "new_game", scopa_application_new_game_action },
};

static void
scopa_application_init (ScopaApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self),
	                                 app_actions,
	                                 G_N_ELEMENTS (app_actions),
	                                 self);
	gtk_application_set_accels_for_action (GTK_APPLICATION (self),
	                                       "app.quit",
	                                       (const char *[]) { "<control>q", NULL });
}
