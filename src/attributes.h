#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "common.h"

static inline uint16_t Attribute_name_index(void *head) {
    return UINT16_AT(head, 0);
}

static inline uint32_t Attribute_length(void *head) {
    return UINT32_AT(head, 2);
}

static inline uint8_t *Attribute_info(void *head) {
    return POINTER_AT(head, 6);
}

#endif
