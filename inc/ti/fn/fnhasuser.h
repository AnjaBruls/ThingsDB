#include <ti/fn/fn.h>

static int do__f_has_user(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    const int nargs = langdef_nd_n_function_params(nd);
    _Bool has_user;
    ti_raw_t * uname;

    if (fn_not_thingsdb_scope("has_user", query, e) ||
        fn_nargs("has_user", DOC_HAS_USER, 1, nargs, e) ||
        ti_access_check_err(
                        ti()->access_thingsdb,
                        query->user, TI_AUTH_GRANT, e) ||
        ti_do_statement(query, nd->children->node, e))
        return e->nr;


    if (!ti_val_is_str(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `has_user` expects argument 1 to be of "
            "type `"TI_VAL_STR_S"` but got type `%s` instead"
            DOC_HAS_USER,
            ti_val_str(query->rval));
        return e->nr;
    }

    uname = (ti_raw_t *) query->rval;
    has_user = !!ti_users_get_by_namestrn(
            (const char *) uname->data,
            uname->n);

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_vbool_get(has_user);

    return e->nr;
}