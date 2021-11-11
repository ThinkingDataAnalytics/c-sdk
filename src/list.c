#include "list.h"
#include <string.h>
#include "util.h"

typedef struct {
    TAListNode *next;
} List_iterator;

List_iterator *list_iterator_new(TAListNode *list) {
    List_iterator *self;
    if (!(self = TA_SAFE_MALLOC(sizeof(List_iterator))))
        return NULL;
    self->next = list;
    return self;
}

TAListNode *list_iterator_next(List_iterator *self) {
    TAListNode *curr = self->next;
    if (curr) {
        self->next = curr->next;
    }
    return curr;
}

void list_iterator_destroy(List_iterator *self) {
    TA_SAFE_FREE(self);
    self = NULL;
}

struct TANode *ta_find_node(const char *key, const struct TANode *parent) {
    List_iterator *it;
    TAListNode *curr;

    if (parent->type != TA_DICT || NULL == key) {
        return NULL;
    }

    it = list_iterator_new(parent->value.child);
    if (it == NULL) {
        return NULL;
    }
    curr = list_iterator_next(it);
    while (NULL != curr) {
        if (NULL != curr->value->key && 0 == strncmp(curr->value->key, key, 256)) {
            list_iterator_destroy(it);
            return curr->value;
        }
        curr = list_iterator_next(it);
    }
    list_iterator_destroy(it);
    return NULL;
}

struct TANode *ta_malloc_node(TANodeType type, const char *key) {
    struct TANode *node = (struct TANode *) TA_SAFE_MALLOC(sizeof(TANode));
    if (node == NULL) {
        return NULL;
    }
    memset(node, 0, sizeof(struct TANode));

    node->type = type;
    if (NULL != key) {
        node->key = ta_strdup(key);
    }

    return node;
}

struct TANode *ta_init_array_node(const char *key) {
    return ta_malloc_node(TA_ARRAY, key);
}

struct TANode *ta_init_dict_node(const char *key) {
    return ta_malloc_node(TA_DICT, key);
}

void ta_remove_node(struct TANode *parent) {
    struct TAListNode *curr = parent->value.child;

    while (NULL != curr) {
        struct TAListNode *next = curr->next;
        ta_free_node(curr->value);
        TA_SAFE_FREE(curr);
        curr = next;
    }
}

void ta_delete_node(const char *key, struct TANode *parent) {
    struct TAListNode *prev = NULL;
    struct TAListNode *curr = parent->value.child;

    while (NULL != curr) {
        struct TAListNode *next = curr->next;
        if (NULL != curr->value->key && 0 == strncmp(curr->value->key, key, 256)) {
            if (NULL == prev) {
                parent->value.child = next;
            } else {
                prev->next = next;
            }
            ta_free_node(curr->value);
            TA_SAFE_FREE(curr);
            break;
        } else {
            prev = curr;
        }
        curr = next;
    }
}

void ta_add_node_copy(struct TANode *node, struct TANode *parent) {
    struct TAListNode *new_list_node;
    TANode *node_new;
    TANodeValue *value;
    char *value_string;

    if (TA_DICT == parent->type) {
        if (NULL == node->key) {
            return;
        }

        ta_delete_node(node->key, parent);
    }

    new_list_node = (struct TAListNode *) TA_SAFE_MALLOC(sizeof(struct TAListNode));
    if (new_list_node == NULL) {
        return;
    }
    new_list_node->next = parent->value.child;
    parent->value.child = new_list_node;

    node_new = ta_malloc_node(value_get_type(node), node->key);
    if (node_new == NULL) {
        return;
    }
    value = (union TANodeValue *) TA_SAFE_MALLOC(sizeof(union TANodeValue));
    if (value == NULL) {
        return;
    }

    switch (node->type) {
        case TA_STRING:
            value_string = (char *) TA_SAFE_MALLOC(strlen(node->value.string_) + 1);
            if (value_string == NULL) {
                return;
            }
            value->string_ = value_string;
            memcpy(value->string_, node->value.string_, strlen(node->value.string_));
            value->string_[strlen(node->value.string_)] = 0;
            node_new->value = *value;
            break;
        case TA_NUMBER:
            value->number_ = node->value.number_;
            node_new->value = *value;
            break;
        case TA_INT:
            value->int_ = node->value.int_;
            node_new->value = *value;
            break;
        case TA_Boolean:
            value->boolean_ = node->value.boolean_;
            node_new->value = *value;
            break;
        case TA_DATE:
            value->date_.seconds = node->value.date_.seconds;
            value->date_.microseconds = node->value.date_.microseconds;
            node_new->value = *value;
            break;
    }
    if (node->type == TA_ARRAY) {
        TAListNode *curr = node->value.child;
        while (NULL != curr) {
            ta_add_node_copy(curr->value, node_new);
            curr = curr->next;
        }
    }

    new_list_node->value = node_new;
    TA_SAFE_FREE(value);
}

struct TAListNode *ta_add_node(struct TANode *node, struct TANode *parent) {
    struct TAListNode *new_node;

    if (TA_DICT == parent->type) {
        if (NULL == node->key) {
            return NULL;
        }

        ta_delete_node(node->key, parent);
    }

    new_node = (struct TAListNode *) TA_SAFE_MALLOC(sizeof(struct TAListNode));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->next = parent->value.child;
    parent->value.child = new_node;
    new_node->value = node;
    return new_node;
}

void ta_destroy_node(struct TANode *node) {
    if (NULL != node->key) {
        TA_SAFE_FREE(node->key);
    }

    switch (node->type) {
        case TA_STRING:
            TA_SAFE_FREE(node->value.string_);
            break;
        case TA_ARRAY:
        case TA_DICT:
            ta_remove_node(node);
            break;
        default:
            break;
    }
    TA_SAFE_FREE(node);
}

void ta_free_node(struct TANode *node) {
    ta_destroy_node(node);
}

TANodeType value_get_type(const TANode *value) {
    return value ? value->type : TA_Error;
}

int value_get_boolean(const TANode *value) {
    return value_get_type(value) == TA_Boolean ? value->value.boolean_ : -1;
}

const char *value_get_string(const TANode *value) {
    return value_get_type(value) == TA_STRING ? value->value.string_ : NULL;
}

double value_get_number(const TANode *value) {
    return value_get_type(value) == TA_NUMBER ? value->value.number_ : 0;
}

long long value_get_int(const TANode *value) {
    return value_get_type(value) == TA_INT ? value->value.int_ : 0;
}

struct TANode *creat_new_node(TANodeType type, const char *key, TANodeValue *value) {
    struct TANode *node_new = ta_malloc_node(type, key);
    if (node_new == NULL) {
        return NULL;
    }
    node_new->value = *value;
    return node_new;
}
