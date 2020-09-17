#include <Python.h>
#include <stdint.h>
#include "membuff.h"

int MemReader_has_error(MemReader *this) {
    return this->pos > this->size;
}

uint32_t MemReader_next_uint32(MemReader *this) {
    if(this->pos >= this->size) {
        this->pos = SIZE_MAX;
        return 0;
    }
    uint32_t result = UINT32(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint16_t MemReader_next_uint16(MemReader *this) {
    if(this->pos >= this->size) {
        this->pos = SIZE_MAX;
        return 0;
    }
    uint16_t result = UINT16(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint8_t MemReader_next_uint8(MemReader *this) {
    if(this->pos >= this->size) {
        this->pos = SIZE_MAX;
        return 0;
    }
    uint8_t result = UINT8(MEMREADER_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint8_t MemReader_peek_uint8(MemReader *this) {
    if(this->pos >= this->size) {
        this->pos = SIZE_MAX;
        return 0;
    }
    return UINT8(MEMREADER_CURR(this));
}

void MemReader_copy_next(MemReader *this, size_t size, void *target) {
    if(this->pos + size > this->size) {
        this->pos = SIZE_MAX;
        return;
    }
    memcpy(target, MEMREADER_CURR(this), size);
    this->pos += size;
}

MemReader *MemReader_from_filename(const char *filename) {
    MemReader *new = PyMem_Malloc(sizeof(MemReader) + 4096);
    new->pos = 0;

    FILE *fp = fopen(filename, "rb");
    new->size = fread(new->data, 1, 4096, fp);
    fclose(fp);

    return new;
}

void MemReader_free(MemReader *this) {
    PyMem_Free(this);
}

void MemReader_print(MemReader *this) {
    printf("Bytes Read: %lu\n", this->pos);
    printf("Total Bytes: %lu\n", this->size);
}
