#include "error.h"

enum javaclass_errcode javaclass_errcode = JAVACLASS_ERR_OK;
const char *javaclass_errmsg = NULL;

void javaclass_error_set(enum javaclass_errcode code, const char *msg) {
    javaclass_errcode = code;
    javaclass_errmsg = msg;
}
