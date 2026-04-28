#ifndef SERIALIZER_H
#define SERIALIZER_H

struct NetHand *serialize_hand(struct Hand * hand);
struct NetTable *serialize_table(struct Table * table);
struct NetCombinationList *serialize_combination_list(struct CombinationNode *list);

#endif