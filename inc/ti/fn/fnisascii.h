#include <ti/fn/fn.h>

#define ISASCII_DOC_ TI_SEE_DOC("#isascii")

static int do__f_isascii(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    _Bool is_ascii;
    ti_raw_t * raw;

    if (fn_chained("isascii", query, e))
        return e->nr;

    if (!langdef_nd_fun_has_one_param(nd))
    {
        int nargs = langdef_nd_n_function_params(nd);
        ex_set(e, EX_NUM_ARGUMENTS,
                "function `isascii` takes 1 argument but %d were given"
                ISASCII_DOC_, nargs);
        return e->nr;
    }

    if (ti_do_statement(query, nd->children->node, e))
        return e->nr;

    raw = (ti_raw_t *) query->rval;
    is_ascii = ti_val_is_raw(query->rval) &&
            strx_is_asciin((const char *) raw->data, raw->n);

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_vbool_get(is_ascii);

    return e->nr;
}
