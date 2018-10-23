/*
 * ti/ex.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ti/ex.h>

static ex_t ex__e;

ex_t * ex_use(void)
{
    ex__e.nr = 0;
    ex__e.n = 0;
    ex__e.msg[0] = '\0';
    return &ex__e;
}

void ex_set(ex_t * e, ex_enum errnr, const char * errmsg, ...)
{
    e->nr = errnr;
    va_list args;
    va_start(args, errmsg);
    e->n = vsnprintf(e->msg, EX_MAX_SZ, errmsg, args);
    va_end(args);
}

const char * ex_str(ex_enum errnr)
{
    switch (errnr)
    {
    case EX_AUTH_ERROR:
        return "authentication error";
    case EX_FORBIDDEN:
        return "forbidden (access denied)";
    case EX_INDEX_ERROR:
        return "requested resource not found";
    case EX_BAD_DATA:
        return "bad request or invalid data";
    case EX_QUERY_ERROR:
        return "syntax error in query";
    case EX_NODE_ERROR:
        return "node is temporary unable to handle the request";
    case EX_INTERNAL:
        return "undefined internal error";
    case EX_REQUEST_TIMEOUT:
        return "request timed out";
    case EX_REQUEST_CANCEL:
        return "request is cancelled";
    case EX_WRITE_UV:
        return "cannot write to socket";
    case EX_ALLOC:
        return "memory allocation error";
    case EX_SUCCESS:
        return "success";
    }
    assert (0);
    return "unknown";
}