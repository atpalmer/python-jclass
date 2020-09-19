#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "membuff.h"

static int _ensure_can_read(struct membuff *this, size_t size) {
    if(this->pos + size > this->size) {
        this->pos = SIZE_MAX;
        return 0;
    }
    return 1;
}

int membuff_has_error(struct membuff *this) {
    return this->pos > this->size;
}

uint32_t membuff_next_uint32(struct membuff *this) {
    if(!_ensure_can_read(this, sizeof(uint32_t)))
        return 0;
    uint32_t result = UINT32(MEMBUFF_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint16_t membuff_next_uint16(struct membuff *this) {
    if(!_ensure_can_read(this, sizeof(uint16_t)))
        return 0;
    uint16_t result = UINT16(MEMBUFF_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint8_t membuff_next_uint8(struct membuff *this) {
    if(!_ensure_can_read(this, sizeof(uint8_t)))
        return 0;
    uint8_t result = UINT8(MEMBUFF_CURR(this));
    this->pos += sizeof(result);
    return result;
}

uint8_t membuff_peek_uint8(struct membuff *this) {
    if(!_ensure_can_read(this, sizeof(uint8_t)))
        return 0;
    return UINT8(MEMBUFF_CURR(this));
}

void membuff_copy_next(struct membuff *this, size_t size, void *target) {
    if(!_ensure_can_read(this, size))
        return;
    memcpy(target, MEMBUFF_CURR(this), size);
    this->pos += size;
}

int membuff_from_filename(const char *filename, struct membuff **r) {
    *r = malloc(sizeof(struct membuff) + 4096);
    if(!*r)
        return errno;

    (*r)->pos = 0;

    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        int errno_ = errno;
        membuff_free(*r);
        *r = NULL;
        return errno_;
    }
    (*r)->size = fread((*r)->data, 1, 4096, fp);
    fclose(fp);

    return 0;
}

void membuff_free(struct membuff *this) {
    free(this);
}

void membuff_print(struct membuff *this) {
    printf("Bytes Read: %lu\n", this->pos);
    printf("Total Bytes: %lu\n", this->size);
}
