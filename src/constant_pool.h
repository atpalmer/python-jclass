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

struct pool_constant {
    uint8_t tag;
};

struct pool_utf8 {
    uint8_t tag;
    uint16_t length;
    char bytes[];
};

struct pool_class {
    uint8_t tag;
    uint16_t name_index;
};

struct pool_string {
    uint8_t tag;
    uint16_t string_index;
};

struct pool_fieldref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_methodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_interface_methodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

struct pool_name_and_type {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
};

typedef struct {
    uint16_t count;
    struct pool_constant *constants[];
} JavaClassConstantPool;

void constant_pool_parse(MemReader *reader, JavaClassConstantPool **obj);
void JavaClassConstantPool_free(JavaClassConstantPool *this);

#endif
