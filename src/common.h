#ifndef COMMON_H
#define COMMON_H

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))

#endif
