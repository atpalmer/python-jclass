#ifndef MEMBUFF_H
#define MEMBUFF_H

#include <arpa/inet.h>

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))
#define UINT32(p)   (ntohl(*(uint32_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define UINT32_AT(p, i)     UINT32(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))

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

static inline MemReader *MemReader_from_filename(const char *filename) {
    MemReader *new = PyMem_Malloc(sizeof(MemReader) + 4096);
    new->pos = 0;

    FILE *fp = fopen(filename, "rb");
    new->size = fread(new->data, 1, 4096, fp);
    fclose(fp);

    return new;
}

static inline void MemReader_free(MemReader *this) {
    PyMem_Free(this);
}

#endif
