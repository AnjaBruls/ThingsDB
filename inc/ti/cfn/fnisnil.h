#include <ti/cfn/fn.h>

#define ISNIL_DOC_ TI_SEE_DOC("#isnil")

static int cq__f_isnil(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    assert (e->nr == 0);
    assert (nd->cl_obj->tp == CLERI_TP_LIST);

    _Bool is_nil;

    if (!langdef_nd_fun_has_one_param(nd))
    {
        int nargs = langdef_nd_n_function_params(nd);
        ex_set(e, EX_BAD_DATA,
                "function `isnil` takes 1 argument but %d were given"
                ISNIL_DOC_, nargs);
        return e->nr;
    }

    if (ti_cq_scope(query, nd->children->node, e))
        return e->nr;

    is_nil = ti_val_is_nil(query->rval);

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_vbool_get(is_nil);

    return e->nr;
}