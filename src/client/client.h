#ifndef CLIENT_H
#define CLIENT_H

void start_client(char *host, int port);
void run_game(GSocketConnection *connection);

#endif