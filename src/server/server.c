#include <glib.h>
#include <gio/gio.h>

#include "server.h"
#include "game-manager.h"

const int CLIENT_TIMEOUT = 60;
int connection_counter = 0;
GSocketConnection *first_client = NULL;

// Callback function called when a client connects to the server
gboolean incoming_callback  (GSocketService *service, GSocketConnection *connection, GObject *source_object, gpointer user_data) {
  g_object_ref(connection); // Keep the connection alive
  GError *error = NULL;
  GSocketAddress *addr = g_socket_connection_get_remote_address(connection, &error);
  gchar *addr_str = g_socket_connectable_to_string(G_SOCKET_CONNECTABLE(addr));
  g_print("Received connection from client at %s\n", addr_str);
  connection_counter++;

  if (error != NULL){
    g_error("%s\n", error->message);
    return FALSE;
  }

  // Starting the game when the server is full (2 clients connected)
  if(connection_counter >= 2){
    // Stopping the listening service
    g_socket_service_stop(service);
    g_socket_listener_close(G_SOCKET_LISTENER(service));

    // Setting the timeout of the clients sockets
    set_socket_timeout(first_client, CLIENT_TIMEOUT);
    set_socket_timeout(connection, CLIENT_TIMEOUT);

    g_print("The server is full, starting the game...\n");
    start_game(first_client, connection);
  }else{
    first_client = connection;
  }

  return FALSE;
}

// Entry point of the server
void start_server(int server_port) {
  GError *error = NULL;
  GSocketService *service = g_socket_service_new();
  g_socket_listener_add_inet_port((GSocketListener*)service, server_port, NULL, &error);

  if (error != NULL){
    g_error("%s\n", error->message);
  }

  g_signal_connect(service, "incoming", G_CALLBACK (incoming_callback),NULL);

  // Starting the socket service
  g_socket_service_start(service);

  g_print("Server is listening on port %i\n", server_port);
}

// Function that sets the timeout of a socket
void set_socket_timeout(GSocketConnection *conn, guint seconds) {
    GSocket *socket = g_socket_connection_get_socket(conn);
    g_socket_set_timeout(socket, seconds);
}