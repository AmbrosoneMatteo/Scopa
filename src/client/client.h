#ifndef CLIENT_H
#define CLIENT_H

extern GAsyncQueue *player_card_queue;
extern GAsyncQueue *player_combo_queue;
extern bool is_network_game;

void start_client(char *host, int port);
void run_game(GSocketConnection *connection);

#endif