#ifndef JCLASS_MEM_H
#define JCLASS_MEM_H

#include <stdlib.h>
#include <jclass/error.h>

static inline void *mem_malloc(size_t s) {
    void *result = malloc(s);
    if(!result) {
        javaclass_error_set(JAVACLASS_ERR_MEMORY, strerror(errno));
        return NULL;
    }
    return result;
}

static inline void *mem_calloc(size_t c, size_t s) {
    void *result = calloc(c, s);
    if(!result) {
        javaclass_error_set(JAVACLASS_ERR_MEMORY, strerror(errno));
        return NULL;
    }
    return result;
}

static inline void mem_free(void *m) {
    if(!m)
        return;
    free(m);
}

#endif
