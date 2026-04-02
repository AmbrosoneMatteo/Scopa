#ifndef SERVER_H
#define SERVER_H

void start_server(void);
gboolean incoming_callback  (GSocketService *service, GSocketConnection *connection, GObject *source_object, gpointer user_data);
void set_socket_timeout(GSocketConnection *conn, guint seconds);

#endif
