#include <ti/fn/fn.h>

static int do__f_set_quota(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    const int nargs = langdef_nd_n_function_params(nd);
    ti_quota_enum_t qtp;
    size_t quota;
    ti_raw_t * rquota;
    ti_collection_t * collection;
    ti_task_t * task;
    uint64_t collection_id;

    if (fn_not_thingsdb_scope("set_quota", query, e) ||
        fn_nargs("set_quota", DOC_SET_QUOTA, 3, nargs, e) ||
        ti_do_statement(query, nd->children->node, e))
        return e->nr;

    collection = ti_collections_get_by_val(query->rval, e);
    if (e->nr)
        return e->nr;

    collection_id = collection->root->id;

    ti_val_drop(query->rval);
    query->rval = NULL;

    if (ti_do_statement(query, nd->children->next->next->node, e))
        return e->nr;

    if (!ti_val_is_str(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `quota` expects argument 2 to be of "
            "type `"TI_VAL_STR_S"` but got type `%s` instead"DOC_SET_QUOTA,
            ti_val_str(query->rval));
        return e->nr;
    }

    rquota = (ti_raw_t *) query->rval;
    qtp = ti_qouta_tp_from_strn((const char *) rquota->data, rquota->n, e);
    if (e->nr)
        return e->nr;

    ti_val_drop(query->rval);
    query->rval = NULL;

    if (ti_do_statement(query, nd->children->next->next->next->next->node, e))
        return e->nr;

    if (!ti_val_is_int(query->rval) && !ti_val_is_nil(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `quota` expects argument 3 to be of type `"TI_VAL_INT_S
            "` or "TI_VAL_NIL_S"` but got type `%s` instead"DOC_SET_QUOTA,
            ti_val_str(query->rval));
        return e->nr;
    }

    quota = (size_t) (ti_val_is_nil(query->rval)
            ? TI_QUOTA_NOT_SET
            : ((ti_vint_t *) query->rval)->int_ < 0
            ? 0
            : ((ti_vint_t *) query->rval)->int_);

    task = ti_task_get_task(query->ev, ti()->thing0, e);
    if (!task)
        return e->nr;

    if (ti_task_add_set_quota(task, collection_id, qtp, quota))
        ex_set_mem(e);  /* task cleanup is not required */
    else
        ti_collection_set_quota(collection, qtp, quota);

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_nil_get();

    return e->nr;
}
