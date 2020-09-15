#ifndef MEMBUFF_H
#define MEMBUFF_H

typedef struct {
    size_t size;
    size_t pos;
    uint8_t data[];
} MemReader;

#endif
