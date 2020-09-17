#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include "membuff.h"

enum constant_tag {
    CONSTANT_TAG_Utf8= 1,
    CONSTANT_TAG_Class = 7,
    CONSTANT_TAG_String = 8,
    CONSTANT_TAG_Fieldref = 9,
    CONSTANT_TAG_Methodref = 10,
    CONSTANT_TAG_InterfaceMethodref = 11,
    CONSTANT_TAG_NameAndType = 12,
};

struct pool_CONSTANT {
    uint8_t tag;
};

struct pool_Utf8 {
    uint8_t tag;
    uint16_t length;
    char bytes[];
};

struct pool_Class {
    uint8_t tag;
    uint16_t name_index;
};

struct pool_String {
    uint8_t tag;
    uint16_t string_index;
};

struct pool_Fieldref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_Methodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_InterfaceMethodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_NameAndType {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
};

struct constant_pool {
    uint16_t count;
    struct pool_CONSTANT *items[];
};

void constant_pool_parse(MemReader *reader, struct constant_pool **obj);
void constant_pool_free(struct constant_pool *this);

#endif
