#include <glib.h>
#include <gio/gio.h>

#include "server.h"
#include "game-manager.h"

const int SRV_PORT = 8888;
int connection_counter = 0;
GSocketConnection *first_client = NULL;

// Callback function called when a client connects to the server
gboolean incoming_callback  (GSocketService *service, GSocketConnection *connection, GObject *source_object, gpointer user_data) {
  GError *err = NULL;
  GSocketAddress *addr = g_socket_connection_get_remote_address(connection, &err);
  gchar *addr_str = g_socket_connectable_to_string(G_SOCKET_CONNECTABLE(addr));
  g_print("Received connection from client at %s\n", addr_str);
  connection_counter++;

  // Starting the game when the server is full (2 clients connected)
  if(connection_counter >= 2){
    // Stopping the listening service
    g_socket_service_stop(service);
    g_socket_listener_close(G_SOCKET_LISTENER(service));

    g_print("The server is full, starting the game...\n");
    start_game(first_client, connection);

    return FALSE;
  }else{
    first_client = connection;
  }
  return FALSE;
}

void start_server(void) {
  g_type_init();
  GError *error = NULL;
  GSocketService *service = g_socket_service_new();
  g_socket_listener_add_inet_port((GSocketListener*)service, SRV_PORT, NULL, &error);

  if (error != NULL){
      g_printerr("%s\n", error->message);
  }

  g_signal_connect(service, "incoming", G_CALLBACK (incoming_callback),NULL);

  // Starting the socket service
  g_socket_service_start(service);

  g_print("Server is listening on port %i\n", SRV_PORT);
}
