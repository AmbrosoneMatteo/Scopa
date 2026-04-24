#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "engine/game-assets.h"
#include "netutils/net-assets.h"
#include "netutils/serializer.h"
#include "netutils/communication.h"
// Function that sends a packet to the client through the output stream
// The function takes as parameters the output stream, the type of the packet,
// the payload and the length of the payload
void send_packet(GOutputStream *out, enum MsgType type, void *payload, int payload_length) {
  struct GamePacket header = { .type = type, .payload_length = payload_length };
  // Sending the header (8 bytes) before the payload
  g_output_stream_write_all(out, &header, sizeof(header), NULL, NULL, NULL);

  // Sending the payload if set
  if (payload_length > 0 && payload != NULL) {
    g_output_stream_write_all(out, payload, payload_length, NULL, NULL, NULL);
  }
}

// Functions that sends to the player the player's hand after it gets serialized
void send_packet_hand(GOutputStream *out, struct Hand *hand){
  struct NetHand *net_hand = serialize_hand(hand);
  send_packet(out, SET_HAND, net_hand, sizeof(struct NetHand));
  free(net_hand);
}

// Functions that sends to the player the game table after it gets serialized
void send_packet_table(GOutputStream *out, struct Table *table){
  struct NetTable *net_table = serialize_table(table);
  send_packet(out, UPDATE_TABLE, net_table, sizeof(struct NetTable));
  free(net_table);
}

// Function that waits for a packet from the client
// The function takes as parameters the input stream and a pointer
// to the header of the packet received
void* receive_packet(GInputStream *in, struct GamePacket *out_header) {
  gsize bytes_read;
  GError *error = NULL;

  // Read the header packet from the stream (8 bytes)
  gboolean success = g_input_stream_read_all(
    in, 
    out_header, 
    sizeof(struct GamePacket), 
    &bytes_read, 
    NULL, 
    &error
  );

    if (!success) {
      if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
        g_error("Player timeout exceeded\n");
      }
      g_error_free(error);
      return NULL;
    }

  void *payload = NULL;
  // Reading the payload (played card)
  if (out_header->payload_length > 0) {
    payload = malloc(out_header->payload_length);
    if(payload == NULL){
      return NULL;
    }
    success = g_input_stream_read_all(
      in, 
      payload, 
      out_header->payload_length, 
      &bytes_read, 
      NULL, 
      &error
    );

    if (!success) {
      g_free(payload);
      return NULL;
    }
  }
  return payload;
}