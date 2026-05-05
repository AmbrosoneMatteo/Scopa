#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "netutils/net-assets.h"
#include "netutils/communication.h"
#include "netutils/serializer.h"
#include "client-ui-manager.h"
#include "client.h"

// Queue to pass the card dropped by the player from
// the GUI thread to the client thread
GAsyncQueue *player_card_queue = NULL;
// Queue to manage the combination selected by the player from the dedicated dialog
GAsyncQueue *player_combo_queue = NULL;
// Variable to define if the network client is running or if the game is local
bool is_network_game = false;

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
    if(player_card_queue == NULL){
        // Creating the queue
        player_card_queue = g_async_queue_new();
    }
    if(player_combo_queue == NULL){
        player_combo_queue = g_async_queue_new();
    }
    is_network_game = true;
    g_print("Client successfully connected to the server\n");

    bool game_over = false;
    int opponent_cards_count = 3;
    struct Hand *hand = NULL;
    update_ui_disable_cards();
    while(!game_over){
        struct GamePacket header;
        void *payload = receive_packet(in, &header);
        if(header.payload_length > 0 && payload == NULL){
            g_print("CLIENT: Error receiving packet\n");
            return;
        }

        // Checking the packet header to determinate the action to perform
        switch(header.type){
            case INIT:
                update_ui_opponent_hand_cards_count(opponent_cards_count);
                break;
            case REQ_CARD:
                update_ui_enable_cards();
                // Waiting for the player card to be received in the queue
                gpointer queue_card = g_async_queue_pop(player_card_queue);
                int *index = (int*)queue_card;
                // Sending played card back to the server
                send_packet_playcard(out, hand->cards[*index]);
                free(index);
                update_ui_disable_cards();
                break;
            case SET_HAND:
                payload = (struct NetHand*)payload;
                hand = deserialize_hand(payload);
                update_ui_hand(hand);
                if(hand->count == 3){
                    opponent_cards_count = 3;
                    update_ui_opponent_hand_cards_count(opponent_cards_count);
                }
                break;
            case UPDATE_TABLE:
                payload = (struct NetTable*)payload;
                struct Table *table = deserialize_table(payload);
                update_ui_table(table);
                break;
            case OPPONENT_CARD:
                opponent_cards_count--;
                update_ui_opponent_hand_cards_count(opponent_cards_count);
                break;
            case REQ_COMBO:
                payload = (struct NetCombinationList*)payload;
                struct CombinationNode *combo_list = deserialize_combination_list(payload);
                update_ui_show_combinations_dialog(combo_list);
                // Waiting for the player to choice a combination
                gpointer queue_combo = g_async_queue_pop(player_combo_queue);
                int *combo_index = (int*)queue_combo;
                // Sending selected combination index to the server
                send_packet_playcombo(out, *combo_index);
                free(combo_index);
                break;
            case UPDATE_PILE:
                struct NetUpdatePile *net_pile = (struct NetUpdatePile*)payload;
                struct Card *card = calloc(1, sizeof(struct Card));
                *card = net_pile->card;
                update_ui_place_on_pile(card, net_pile->is_opponent_pile);
                break;
            case GAME_END:
                struct NetEndGameData *net_endgame = (struct NetEndGameData*)payload;
                struct CardNode *player_pile = deserialize_pile(&net_endgame->player_pile);
                struct CardNode *opponent_pile = deserialize_pile(&net_endgame->opponent_pile);
                update_ui_endgame_dialog(player_pile, opponent_pile, net_endgame->player_scope, net_endgame->opponent_scope);
                game_over = true;
                break;
        }

        free(payload);
    }
}