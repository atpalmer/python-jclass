#ifndef JCLASS_MEMBUFF_H
#define JCLASS_MEMBUFF_H

#include <stdlib.h>

struct membuff {
    size_t size;
    size_t pos;
    uint8_t data[];
};

#define MEMBUFF_CURR(r)   (&(r)->data[(r)->pos])

int membuff_has_error(struct membuff *this);
uint32_t membuff_next_uint32(struct membuff *this);
uint16_t membuff_next_uint16(struct membuff *this);
uint8_t membuff_next_uint8(struct membuff *this);
uint8_t membuff_peek_uint8(struct membuff *this);
void membuff_copy_next(struct membuff *this, size_t size, void *target);
int membuff_from_filename(const char *filename, struct membuff **r);
void membuff_free(struct membuff *this);
void membuff_print(struct membuff *this);

#endif
