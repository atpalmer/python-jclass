#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <string.h>

enum javaclass_errcode {
    JAVACLASS_ERR_OK = 0,
    JAVACLASS_ERR_MEMORY,
    JAVACLASS_ERR_OS,
    JAVACLASS_ERR_CAFEBABE,
    JAVACLASS_ERR_BADVER,
    JAVACLASS_ERR_PARSE,
};

void javaclass_error_clear(void);
void javaclass_error_set(enum javaclass_errcode code, const char *msg);
enum javaclass_errcode javaclass_error_get(const char **msg);

#endif
