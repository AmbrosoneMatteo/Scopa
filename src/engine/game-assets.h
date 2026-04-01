#ifndef GAME_ASSETS
#define GAME_ASSETS
#define DECK_SIZE 40
#define HAND_SIZE 3 // Number of cards in a player's hand
#define MAX_TABLE_SIZE 12

enum Suit {
  CLUBS,
  DIAMONDS,
  HEARTS,
  SPADES
};

// Struct that represents a card
struct Card {
  enum Suit suit;
  int value; // 1-10
};

// Struct that represents the deck
struct Deck {
  struct Card cards[DECK_SIZE];
  struct Card *top; // Pointer to the card on top of the deck
};

// Struct that represents the table where the cards are played
struct Table {
  struct Card cards[MAX_TABLE_SIZE];
  int count; // Keeping track of how many cards are there on the table
};

// Struct that represents the player's hand
struct Hand {
  struct Card cards[HAND_SIZE];
  int count; // Keeping track of how many cards are there in the player's hand
};
#endif
