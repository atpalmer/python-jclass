#ifndef FIELDS_H
#define FIELDS_H

#import "common.h"


static inline uint16_t Field_access_flags(void *head) {
    return UINT16_AT(head, 0);
}

static inline uint16_t Field_name_index(void *head) {
    return UINT16_AT(head, 2);
}

static inline uint16_t Field_descriptor_index(void *head) {
    return UINT16_AT(head, 4);
}

static inline uint16_t Field_attributes_count(void *head) {
    return UINT16_AT(head, 6);
}

static inline uint8_t *Field_attributes(void *head) {
    return POINTER_AT(head, 8);
}

#endif
