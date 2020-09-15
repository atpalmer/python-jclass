#ifndef MEMBUFF_H
#define MEMBUFF_H

#include "parse.h"

typedef struct {
    size_t size;
    size_t pos;
    uint8_t data[];
} MemReader;

#define MEMREADER_CURR(r)   (&(r)->data[(r)->pos])

/* TODO: bounds-checking and error handling... */

static inline uint32_t MemReader_next_uint32(MemReader *this) {
    uint32_t result = UINT32(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

static inline uint16_t MemReader_next_uint16(MemReader *this) {
    uint16_t result = UINT16(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

static inline uint8_t MemReader_next_uint8(MemReader *this) {
    uint8_t result = UINT8(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

static inline uint8_t MemReader_peek_uint8(MemReader *this) {
    return UINT8(MEMREADER_CURR(this));
}

static inline void MemReader_copy_next(MemReader *this, size_t size, void *target) {
    memcpy(target, MEMREADER_CURR(this), size);
    this->pos += size;
}

#endif
