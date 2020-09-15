#ifndef PARSE_H
#define PARSE_H

#include <arpa/inet.h>

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))
#define UINT32(p)   (ntohl(*(uint32_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define UINT32_AT(p, i)     UINT32(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))


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


#endif
