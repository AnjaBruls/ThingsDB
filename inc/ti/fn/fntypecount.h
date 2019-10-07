#include <ti/fn/fn.h>

static int do__f_type_count(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    const int nargs = langdef_nd_n_function_params(nd);
    ti_type_t * type;
    size_t n;

    if (fn_not_collection_scope("type_count", query, e) ||
        fn_nargs("type_count", DOC_TYPE_INFO, 1, nargs, e) ||
        ti_do_statement(query, nd->children->node, e))
        return e->nr;

    if (!ti_val_is_str(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `type_count` expects argument 1 to be of "
            "type `"TI_VAL_STR_S"` but got type `%s` instead"
            DOC_TYPE_INFO,
            ti_val_str(query->rval));
        return e->nr;
    }

    type = ti_types_by_raw(query->collection->types, (ti_raw_t *) query->rval);
    if (!type)
        return ti_raw_err_not_found((ti_raw_t *) query->rval, "type", e);

    n = ti_collection_ntype(query->collection, type);

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_vint_create((int64_t) n);

    if (!query->rval)
        ex_set_mem(e);

    return e->nr;
}
