#include <string.h>
#include "json.h"
#include "list.h"
#include "util.h"

char *print_string(const char *str) {
    const char *ptr;
    char *ptr2, *out;
    int len = 0;
    unsigned char token;
    if (!str)
        return NULL;
    ptr = str;
    while ((token = *ptr) && ++len) {
        if (strchr("\"\\\b\f\n\r\t", token))
            len++;
        else if (token < 32)
            len += 5;
        ptr++;
    }
    
    out = (char *)TA_SAFE_MALLOC(len + 3);
    if (!out)
        return 0;

    ptr2 = out;
    ptr = str;
    *ptr2++ = '\"';
    while (*ptr) {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\') {
            *ptr2++ = *ptr++;
        } else {
            *ptr2++ = '\\';
            switch (token = *ptr++) {
                case '\\':
                    *ptr2++ = '\\';
                    break;
                case '\"':
                    *ptr2++ = '\"';
                    break;
                case '\b':
                    *ptr2++ = 'b';
                    break;
                case '\f':
                    *ptr2++ = 'f';
                    break;
                case '\n':
                    *ptr2++ = 'n';
                    break;
                case '\r':
                    *ptr2++ = 'r';
                    break;
                case '\t':
                    *ptr2++ = 't';
                    break;
                default:
                    sprintf(ptr2, "u%04x", token);
                    ptr2 += 5;
                    break;
            }
        }
    }
    *ptr2++ = '\"';
    *ptr2++ = 0;
    return out;
}

static char *print_array(const struct TANode *node, int depth) {
    char **entries = 0;
    char *out = 0, *ptr , *ret;
    int len = 5;
    struct TAListNode *child = node->value.child;
    int numentries = 0, i = 0, fail = 0;
    
    while (child) {
        numentries++;
        child = child->next;
    }
    
    entries = (char**)TA_SAFE_MALLOC(numentries * sizeof(char *));
    if (!entries)
        return 0;
    
    memset(entries,0,numentries*sizeof(char*));
    
    child = node->value.child;
    while (child && !fail) {
        entries[i++] = ret = print_node(child->value, depth);
        if (ret)
            len += strlen(ret) + 2;
        else
            fail = 1;
        child = child->next;
    }
    
    if (!fail)
        out = (char *)TA_SAFE_MALLOC(len);
    if (!out)
        fail = 1;

    if (fail) {
        for (i = 0 ; i < numentries ; i++) {
            if (entries[i])
                TA_SAFE_FREE(entries[i]);
        }
        TA_SAFE_FREE(entries);
        return 0;
    }
    
    *out = '[';
    ptr = out + 1;
    *ptr = 0;
    for (i = 0 ; i < numentries ; i++) {
        strcpy(ptr, entries[i]);
        ptr += strlen(entries[i]);
        if (i != numentries - 1) {
            *ptr++ = ',';
            *ptr = 0;
        }
        TA_SAFE_FREE(entries[i]);
    }
    TA_SAFE_FREE(entries);
    *ptr++ = ']';
    *ptr++ = 0;
    return out;
}

char *print_object(const struct TANode *node, int depth) {
    char **entries = 0, **names = 0;
    char *out = 0, *ptr, *ret, *str;
    int len = 7, i = 0;
    struct TAListNode *child = node->value.child;
    int numentries = 0, fail = 0;
    while (child) {
        numentries++;
        child = child->next;
    }
    entries = (char**)TA_SAFE_MALLOC(numentries * sizeof(char *));
    if (!entries)
        return 0;
    names = (char**)TA_SAFE_MALLOC(numentries * sizeof(char *));
    if (!names) {
        TA_SAFE_FREE(entries);
        return 0;
    }
    
    memset(entries, 0, sizeof(char *) * numentries);
    memset(names, 0, sizeof(char *) * numentries);

    child = node->value.child;
    depth++;
    while (child) {
        names[i] = str = print_string(child->value->key);
        entries[i++] = ret = print_node(child->value, depth);
        if (str && ret)
            len += strlen(ret) + strlen(str) + 2;
        else
            fail = 1;
        child = child->next;
    }
    
    if (!fail)
        out = (char *)TA_SAFE_MALLOC(len);
    if (!out)
        fail = 1;

    if (fail) {
        for (i = 0 ; i < numentries ; i++) {
            if (names[i])
                TA_SAFE_FREE(names[i]);
            if (entries[i])
                TA_SAFE_FREE(entries[i]);
        }
        TA_SAFE_FREE(names);
        TA_SAFE_FREE(entries);
        return 0;
    }
    
    *out = '{';
    ptr = out + 1;
    for (i = 0 ; i < numentries ; i++) {
        strcpy(ptr, names[i]);
        ptr += strlen(names[i]);
        *ptr++ = ':';
        strcpy(ptr, entries[i]);
        ptr += strlen(entries[i]);
        if (i != numentries - 1)
            *ptr++ = ',';
        TA_SAFE_FREE(names[i]);
        TA_SAFE_FREE(entries[i]);
    }
    
    TA_SAFE_FREE(names);
    TA_SAFE_FREE(entries);
    *ptr++ = '}';
    *ptr++ = 0;
    
    return out;
}

char *print_node(const struct TANode *node, int depth) {
    if (NULL == node) {
        return NULL;
    }
    
    char *out = NULL;
    
    switch(node->type) {
        case TA_NUMBER:
            out = TA_SAFE_MALLOC(64);
            snprintf(out, 64, "%.3f", value_get_number(node));
            break;
        case TA_INT:
            out = TA_SAFE_MALLOC(64);
            snprintf(out, 64, "%lld", value_get_int(node));
            break;
        case TA_DATE:
        {
            char *time = convert_time_to_string(node);
            out = (char *)TA_SAFE_MALLOC(64);
            snprintf(out, 64, "\"%s\"", time);
            TA_SAFE_FREE(time);
        }
            break;
        case TA_Boolean:
            if (value_get_boolean(node)) {
                out = ta_strdup("true");
            } else {
                out = ta_strdup("false");
            }
            break;
        case TA_STRING:
            out = print_string(value_get_string(node));
            break;
        case TA_ARRAY:
            out = print_array(node, depth);
            break;
        case TA_DICT:
            out = print_object(node, depth);
            break;
        default:
            return NULL;
    }
    
    return out;
}
