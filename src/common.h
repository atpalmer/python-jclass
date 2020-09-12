#ifndef COMMON_H
#define COMMON_H

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

#endif
