#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <string.h>

enum javaclass_errcode {
    JAVACLASS_ERR_OK = 0,
    JAVACLASS_ERR_MEMORY,
    JAVACLASS_ERR_OS,
};

extern enum javaclass_errcode javaclass_errcode;
extern const char *javaclass_errmsg;

void javaclass_error_set(enum javaclass_errcode code, const char *msg);

#endif
