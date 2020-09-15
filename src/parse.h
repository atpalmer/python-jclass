#ifndef PARSE_H
#define PARSE_H

#include <arpa/inet.h>
#include "membuff.h"

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))
#define UINT32(p)   (ntohl(*(uint32_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define UINT32_AT(p, i)     UINT32(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))


static inline uint32_t MemReader_uint32(MemReader *this) {
    uint32_t result = UINT32(&this->data[this->pos]);
    this->pos += sizeof(result);
    return result;
}

static inline size_t parse32(void *data, uint32_t *target) {
    *target = UINT32(data);
    return 4;
}

static inline size_t parse16(void *data, uint16_t *target) {
    *target = UINT16(data);
    return 2;
}


/* CONSTANT POOL */

/* common header byte */

static inline uint8_t Constant_tag(void *head) {
    return UINT8_AT(head, 0);
}


/* 1 = Utf8 */

static inline uint16_t Utf8_length(void *head) {
    return UINT16_AT(head, 1);
}

static inline char *Utf8_bytes(void *head) {
    return POINTER_AT(head, 3);
}


/* 7 = Class */

static inline uint16_t Class_name_index(void *head) {
    return UINT16_AT(head, 1);
}


/* 8 = String */

static inline uint16_t String_string_index(void *head) {
    return UINT16_AT(head, 1);
}


/* 9 = Fieldref */

static inline uint16_t Fieldref_class_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t Fieldref_name_and_type_index(void *head) {
    return UINT16_AT(head, 3);
}


/* 10 = Methodref */

static inline uint16_t Methodref_class_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t Methodref_name_and_type_index(void *head) {
    return UINT16_AT(head, 3);
}


/* 12 = NameAndType */

static inline uint16_t NameAndType_name_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t NameAndType_descriptor_index(void *head) {
    return UINT16_AT(head, 3);
}


/* FIELDS */

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


/* METHODS */

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


/* ATTRIBUTES */

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
