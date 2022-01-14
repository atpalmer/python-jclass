#ifndef JCLASS_MEMBUFF_H
#define JCLASS_MEMBUFF_H

#include <arpa/inet.h>

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))
#define UINT32(p)   (ntohl(*(uint32_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define UINT32_AT(p, i)     UINT32(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))

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
