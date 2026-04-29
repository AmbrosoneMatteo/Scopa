#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "netutils/net-assets.h"
#include "netutils/communication.h"
#include "netutils/serializer.h"
#include "client-ui-manager.h"
#include "client.h"

// Function that starts the client socket and connects to
// the peer hosting the server
void start_client(char *host, int port){
    GError *error = NULL;
    GSocketClient *client = g_socket_client_new();
    GSocketConnection *connection = g_socket_client_connect_to_host(
        client,
        host,
        port,
        NULL,
        &error
    );

    if(error != NULL){
        g_error("Error connecting to server: %s\n", error->message);
        g_object_unref(connection);
        g_object_unref(client);
        return;
    }

    run_game(connection); // Running the game listening loop

    g_object_unref(connection);
    g_object_unref(client);
}

// Function that listens to instructions from the server
// and executes them
void run_game(GSocketConnection *connection){
    GInputStream *in = g_io_stream_get_input_stream(G_IO_STREAM (connection));
    GOutputStream *out = g_io_stream_get_output_stream(G_IO_STREAM (connection));
    g_print("Client successfully connected to the server\n");

    bool game_over = false;
    while(!game_over){
        struct GamePacket header;
        void *payload = receive_packet(in, &header);
        if(header.payload_length > 0 && payload == NULL){
            g_print("Error receiving packet\n");
            return;
        }

        switch(header.type){
            case INIT:
                g_print("Game started\n");
                break;
            case REQ_CARD:
                g_print("REQ_CARD\n");
                break;
            case SET_HAND:
                g_print("SET_HAND\n");
                payload = (struct NetHand*)payload;
                struct Hand *hand = deserialize_hand(payload);
                update_ui_hand(hand);
                break;
            case UPDATE_TABLE:
                g_print("UPDATE_TABLE\n");
                payload = (struct NetTable*)payload;
                struct Table *table = deserialize_table(payload);
                update_ui_table(table);
                break;
            case OPPONENT_CARD:
                g_print("OPPONENT_CARD\n");
                break;
            case REQ_COMBO:
                g_print("REQ_COMBO\n");
                break;
        }

        free(payload);
    }
}