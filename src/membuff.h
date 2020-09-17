#ifndef MEMBUFF_H
#define MEMBUFF_H

#include <Python.h>
#include <arpa/inet.h>

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))
#define UINT32(p)   (ntohl(*(uint32_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define UINT32_AT(p, i)     UINT32(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))

static inline void *MALLOC(size_t n) {
    void *p = PyMem_Malloc(n);
    if(!p)
        PyErr_NoMemory();
    return p;
}

static inline void FREE(void *p) {
    PyMem_Free(p);
}

typedef struct {
    size_t size;
    size_t pos;
    uint8_t data[];
} MemReader;

#define MEMREADER_CURR(r)   (&(r)->data[(r)->pos])

int MemReader_has_error(MemReader *this);
uint32_t MemReader_next_uint32(MemReader *this);
uint16_t MemReader_next_uint16(MemReader *this);
uint8_t MemReader_next_uint8(MemReader *this);
uint8_t MemReader_peek_uint8(MemReader *this);
void MemReader_copy_next(MemReader *this, size_t size, void *target);
MemReader *MemReader_from_filename(const char *filename);
void MemReader_free(MemReader *this);
void MemReader_print(MemReader *this);

#endif
