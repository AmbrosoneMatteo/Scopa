#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "engine/game-assets.h"
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