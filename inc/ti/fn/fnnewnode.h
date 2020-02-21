#include <ti/fn/fn.h>

static int do__f_new_node(ti_query_t * query, cleri_node_t * nd, ex_t * e)
{
    const int nargs = langdef_nd_n_function_params(nd);
    char salt[CRYPTX_SALT_SZ];
    char encrypted[CRYPTX_SZ];
    char * secret;
    ti_node_t * node;
    ti_raw_t * rsecret;
    ti_raw_t * raddr;
    ti_task_t * task;
    cleri_children_t * child = nd->children;
    struct in_addr sa;
    struct in6_addr sa6;
    struct sockaddr_storage addr;
    char * addrstr;
    int port;

    if (fn_not_thingsdb_scope("new_node", query, e) ||
        fn_nargs_range("new_node", DOC_NEW_NODE, 2, 3, nargs, e))
        return e->nr;

    if (ti_do_statement(query, child->node, e))
        return e->nr;

    if (!ti_val_is_str(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `new_node` expects argument 1 to be of "
            "type `"TI_VAL_STR_S"` but got type `%s` instead"DOC_NEW_NODE,
            ti_val_str(query->rval));
        return e->nr;
    }

    rsecret = (ti_raw_t *) query->rval;
    if (!rsecret->n || !strx_is_graphn((char *) rsecret->data, rsecret->n))
    {
        ex_set(e, EX_VALUE_ERROR,
            "a `secret` is required "
            "and should only contain graphical characters"DOC_NEW_NODE);
        return e->nr;
    }

    secret = ti_raw_to_str(rsecret);
    if (!secret)
    {
        ex_set_mem(e);
        return e->nr;
    }

    ti_val_drop(query->rval);
    query->rval = NULL;

    if (ti_do_statement(query, (child = child->next->next)->node, e))
        goto fail0;

    if (!ti_val_is_str(query->rval))
    {
        ex_set(e, EX_TYPE_ERROR,
            "function `new_node` expects argument 2 to be of "
            "type `"TI_VAL_STR_S"` but got type `%s` instead"DOC_NEW_NODE,
            ti_val_str(query->rval));
        goto fail0;
    }

    raddr = (ti_raw_t *) query->rval;
    if (raddr->n >= INET6_ADDRSTRLEN)
    {
        ex_set(e, EX_VALUE_ERROR, "invalid IPv4/6 address: `%.*s`",
                (int) raddr->n,
                (char *) raddr->data);
        goto fail0;
    }

    addrstr = ti_raw_to_str(raddr);
    if (!addrstr)
    {
        ex_set_mem(e);
        goto fail0;
    }

    if (nargs == 3)
    {
        int64_t iport;
        ti_val_drop(query->rval);
        query->rval = NULL;

        /* Read the port number from arguments */
        if (ti_do_statement(query, (child = child->next->next)->node, e))
            goto fail1;

        if (!ti_val_is_int(query->rval))
        {
            ex_set(e, EX_TYPE_ERROR,
                "function `new_node` expects argument 3 to be of "
                "type `"TI_VAL_INT_S"` but got type `%s` instead"DOC_NEW_NODE,
                ti_val_str(query->rval));
            goto fail1;
        }

        iport = VINT(query->rval);
        if (iport < 1<<0 || iport >= 1<<16)
        {
            ex_set(e, EX_VALUE_ERROR,
                "`port` should be an integer value between 1 and 65535, "
                "got %"PRId64,
                iport);
            goto fail1;
        }
        port = (int) iport;
    }
    else
    {
        /* Use default port number as no port is given as argument */
        port = TI_DEFAULT_NODE_PORT;
    }

    if (inet_pton(AF_INET, addrstr, &sa))  /* try IPv4 */
    {
        if (uv_ip4_addr(addrstr, port, (struct sockaddr_in *) &addr))
        {
            ex_set(e, EX_INTERNAL,
                    "cannot create IPv4 address from `%s:%d`",
                    addrstr, port);
            goto fail1;
        }
    }
    else if (inet_pton(AF_INET6, addrstr, &sa6))  /* try IPv6 */
    {
        if (uv_ip6_addr(addrstr, port, (struct sockaddr_in6 *) &addr))
        {
            ex_set(e, EX_INTERNAL,
                    "cannot create IPv6 address from `[%s]:%d`",
                    addrstr, port);
            goto fail1;
        }
    }
    else
    {
        ex_set(e, EX_VALUE_ERROR, "invalid IPv4/6 address: `%s`", addrstr);
        goto fail1;
    }

    if (ti_nodes_check_add(e))
        goto fail1;

    cryptx_gen_salt(salt);
    cryptx(secret, salt, encrypted);

    task = ti_task_get_task(query->ev, ti()->thing0, e);
    if (!task)
        goto fail1;

    node = ti_nodes_new_node(ti_nodes_next_id(), 0, port, addrstr, encrypted);
    if (!node)
    {
        ex_set_mem(e);
        goto fail1;
    }

    if (ti_task_add_new_node(task, node))
        ex_set_mem(e);  /* task cleanup is not required */

    query->ev->flags |= TI_EVENT_FLAG_SAVE;

    ti_val_drop(query->rval);
    query->rval = (ti_val_t *) ti_vint_create((int64_t) node->id);
    if (!query->rval)
        ex_set_mem(e);

fail1:
    free(addrstr);
fail0:
    free(secret);
    return e->nr;
}
