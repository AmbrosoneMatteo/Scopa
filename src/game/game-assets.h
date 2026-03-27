#ifndef GAME_ASSETS
#define GAME_ASSETS
#define DECK_SIZE 40

enum Suit {
  CLUBS,
  DIAMONDS,
  HEARTS,
  SPADES
};

struct Card {
  enum Suit suit;
  int value; // 1-10
};

// Struct that rapresents the deck
struct Deck {
  struct Card cards[DECK_SIZE];
  struct Card *top; // Pointer to the card on top of the deck
};

#endif