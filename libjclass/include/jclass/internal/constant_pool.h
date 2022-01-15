#ifndef JCLASS_CONSTANT_POOL_H
#define JCLASS_CONSTANT_POOL_H

#include <stdio.h>
#include <string.h>

enum constant_tag {
    CONSTANT_TAG_Utf8 = 1,
    /* no 2 */
    CONSTANT_TAG_Integer = 3,
    CONSTANT_TAG_Float = 4,
    CONSTANT_TAG_Long = 5,
    CONSTANT_TAG_Double = 6,
    CONSTANT_TAG_Class = 7,
    CONSTANT_TAG_String = 8,
    CONSTANT_TAG_Fieldref = 9,
    CONSTANT_TAG_Methodref = 10,
    CONSTANT_TAG_InterfaceMethodref = 11,
    CONSTANT_TAG_NameAndType = 12,
    /* no 13 */
    /* no 14 */
    CONSTANT_TAG_MethodHandle = 15,
    CONSTANT_TAG_MethodType = 16,
    /* no 17 */
    CONSTANT_TAG_InvokeDynamic = 18,
};

/* base */
struct pool_CONSTANT {
    uint8_t tag;
};

/* 1 */
struct pool_Utf8 {
    uint8_t tag;
    uint16_t length;
    char bytes[];
};

/* no 2 */

/* 3 */
struct pool_Integer {
    uint8_t tag;
    int32_t bytes;
};

/* 4 */
struct pool_Float {
    uint8_t tag;
    float bytes;
};

/* 5 */
struct pool_Long {
    uint8_t tag;
    int64_t bytes;
};

/* 6 */
struct pool_Double {
    uint8_t tag;
    double bytes;
};

/* 7 */
struct pool_Class {
    uint8_t tag;
    uint16_t name_index;
};

/* 8 */
struct pool_String {
    uint8_t tag;
    uint16_t string_index;
};

/* 9 */
struct pool_Fieldref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

/* 10 */
struct pool_Methodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

/* 11 */
struct pool_InterfaceMethodref {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
};

/* 12 */
struct pool_NameAndType {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
};

/* no 13 */

/* no 14 */

/* 15 */
struct pool_MethodHandle {
    uint8_t tag;
    uint8_t reference_kind;
    uint16_t reference_index;
};

/* 16 */
struct pool_MethodType {
    uint8_t tag;
    uint16_t descriptor_index;
};

/* no 17 */

/* 18 */
struct pool_InvokeDynamic {
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
};

/* items */
struct constant_pool {
    uint16_t count;
    struct pool_CONSTANT *items[];
};

static inline int pool_Utf8_eq_str(struct pool_Utf8 *utf8, const char *s) {
    if(strlen(s) != utf8->length)
        return 0;
    return memcmp(s, utf8->bytes, utf8->length) == 0;
}

struct constant_pool *constant_pool_parse(FILE *reader);
void constant_pool_free(struct constant_pool *this);

void *constant_pool_item(struct constant_pool *this, uint16_t i);
struct pool_Utf8 *constant_pool_Utf8_item(struct constant_pool *this, uint16_t i);
struct pool_Class *constant_pool_Class_item(struct constant_pool *this, uint16_t i);

#endif
