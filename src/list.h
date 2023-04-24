#ifndef list_h
#define list_h

#include <stdio.h>
#include "thinkingdata_private.h"

struct TANode *ta_init_array_node(const char* key);
struct TANode *ta_init_dict_node(const char *key);
struct TAListNode *ta_add_node(struct TANode *node, struct TANode *parent);
void ta_add_node_copy(struct TANode *node, struct TANode *parent);
void ta_delete_node(const char *key, struct TANode *parent);
void ta_remove_node(struct TANode *parent);
void ta_free_node(struct TANode *node);

struct TANode *ta_find_node(const char *key, const struct TANode *parent);
TANodeType value_get_type(const TANode *value);
int value_get_boolean(const TANode *value);
const char  *value_get_string(const TANode *value);
double value_get_number(const TANode *value);
long value_get_int(const TANode *value);
struct TANode *creat_new_node(TANodeType type, const char *key, TANodeValue *value);

#endif /* list_h */
