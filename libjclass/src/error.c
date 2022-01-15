#include <stdatomic.h>
#include <jclass/error.h>

__thread enum javaclass_errcode javaclass_errcode = JAVACLASS_ERR_OK;
__thread const char *javaclass_errmsg = NULL;

void javaclass_error_clear(void) {
    javaclass_errcode = JAVACLASS_ERR_OK;
    javaclass_errmsg = NULL;
}

void javaclass_error_set(enum javaclass_errcode code, const char *msg) {
    javaclass_errcode = code;
    javaclass_errmsg = msg;
}

enum javaclass_errcode javaclass_error_get(const char **msg) {
    enum javaclass_errcode code = javaclass_errcode;
    *msg = javaclass_errmsg;
    javaclass_error_clear();
    return code;
}
