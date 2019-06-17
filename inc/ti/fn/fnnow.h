static int cq__f_now(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    assert (e->nr == 0);
    assert (nd->cl_obj->tp == CLERI_TP_LIST);

    if (!langdef_nd_fun_has_zero_params(nd))
    {
        int n = langdef_nd_n_function_params(nd);
        ex_set(e, EX_BAD_DATA,
                "function `now` takes 0 arguments but %d %s given",
                n, n == 1 ? "was" : "were");
        return e->nr;
    }

    query->rval = (ti_val_t *) ti_vfloat_create(util_now());
    if (!query->rval)
        ex_set_alloc(e);

    return e->nr;
}
