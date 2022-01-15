#ifndef JCLASS_MEMBUFF_H
#define JCLASS_MEMBUFF_H

#include <stdio.h>
#include <stdint.h>

struct membuff {
    FILE *fh;
};

int membuff_has_error(struct membuff *this);
uint32_t membuff_next_uint32(struct membuff *this);
uint16_t membuff_next_uint16(struct membuff *this);
uint8_t membuff_next_uint8(struct membuff *this);
uint8_t membuff_peek_uint8(struct membuff *this);
void membuff_copy_next(struct membuff *this, size_t size, void *target);
int membuff_from_filename(const char *filename, struct membuff **r);
void membuff_free(struct membuff *this);

#endif
