#ifndef METHODS_H
#define METHODS_H

#include "common.h"

static inline uint16_t Method_access_flags(void *head) {
    return UINT16_AT(head, 0);
}

static inline uint16_t Method_name_index(void *head) {
    return UINT16_AT(head, 2);
}

static inline uint16_t Method_descriptor_index(void *head) {
    return UINT16_AT(head, 4);
}

static inline uint16_t Method_attributes_count(void *head) {
    return UINT16_AT(head, 6);
}

static inline uint8_t *Method_attributes(void *head) {
    return POINTER_AT(head, 8);
}

#endif
