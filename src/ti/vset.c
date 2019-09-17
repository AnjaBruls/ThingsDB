/*
 * ti/vset.c
 */
#include <assert.h>
#include <tiinc.h>
#include <stdlib.h>
#include <ti/vset.h>
#include <ti/val.h>
#include <util/logger.h>

ti_vset_t * ti_vset_create(void)
{
    ti_vset_t * vset = malloc(sizeof(ti_vset_t));
    if (!vset)
        return NULL;

    vset->ref = 1;
    vset->tp = TI_VAL_SET;
    vset->flags = 0;

    vset->imap = imap_create();
    if (!vset->imap)
    {
        free(vset);
        return NULL;
    }

    return vset;
}

void ti_vset_destroy(ti_vset_t * vset)
{
    if (!vset)
        return;
    imap_destroy(vset->imap, (imap_destroy_cb) ti_val_drop);
    free(vset);
}

int ti_vset_to_packer(ti_vset_t * vset, qp_packer_t ** packer, int options)
{
    vec_t * vec = imap_vec(vset->imap);
    if (!vec ||
        qp_add_map(packer) ||
        qp_add_raw(*packer, (const uchar * ) TI_KIND_S_SET, 1) ||
        qp_add_array(packer))
        return -1;
    for (vec_each(vec, ti_thing_t, t))
    {
        if (options < 0)
        {
            if (ti_thing_is_new(t))
            {
                ti_thing_unmark_new(t);
                if (ti_thing_to_packer(t, packer, options))
                    return -1;
                continue;
            }
            if (ti_thing_id_to_packer(t, packer))
                return -1;
            continue;
        }

        if (options > 0)
        {
            if (ti_thing_to_packer(t, packer, options))
                return -1;
            continue;
        }

        if (ti_thing_id_to_packer(t, packer))
            return -1;
    }
    return qp_close_array(*packer) || qp_close_map(*packer);
}

int ti_vset_to_list(ti_vset_t ** vsetaddr)
{
    ti_varr_t * list;
    vec_t * vec = imap_vec_pop((*vsetaddr)->imap);
    if (!vec)
        goto failed;

    list = malloc(sizeof(ti_varr_t));
    if (!list)
        goto failed;

    list->ref = 1;
    list->tp = TI_VAL_ARR;
    list->flags = vec->n ? TI_VFLAG_ARR_MHT : 0;
    list->vec = vec;

    for (vec_each(list->vec, ti_val_t, val))
        ti_incref(val);

    ti_val_drop((ti_val_t *) *vsetaddr);
    *vsetaddr = (ti_vset_t *) list;

    return 0;

failed:
    free(vec);
    return -1;
}

int ti_vset_to_tuple(ti_vset_t ** vsetaddr)
{
    if (ti_vset_to_list(vsetaddr))
        return -1;
    (*vsetaddr)->flags |= TI_VFLAG_ARR_TUPLE;
    return 0;
}

int ti_vset_to_file(ti_vset_t * vset, FILE * f)
{
    vec_t * vec = imap_vec(vset->imap);
    if (    !vec ||
            qp_fadd_type(f, QP_MAP1) ||
            qp_fadd_raw(f, (const uchar * ) TI_KIND_S_SET, 1) ||
            qp_fadd_type(f, vec->n > 5 ? QP_ARRAY_OPEN: QP_ARRAY0 + vec->n))
        return -1;

    for (vec_each(vec, ti_thing_t, t))
        if (ti_thing_id_to_file(t, f))
            return -1;

    return vec->n > 5 ? qp_fadd_type(f, QP_ARRAY_CLOSE) : 0;
}

int ti_vset_assign(ti_vset_t ** vsetaddr)
{
    vec_t * vec;
    ti_vset_t * vset = *vsetaddr;

    if (vset->ref == 1)
        return 0;  /* with only one reference we do not require a copy */

    if (!(vec = imap_vec(vset->imap)) ||
        !(vset = ti_vset_create()))
        return -1;

    for (vec_each(vec, ti_thing_t, t))
    {
        if (ti_vset_add(vset, t))
        {
            ti_vset_destroy(vset);
            return -1;
        }
        ti_incref(t);
    }

    ti_decref(*vsetaddr);
    *vsetaddr = vset;

    return 0;
}

/*
 * Increments the reference for each moved value to the set.
 * The return value is <0 in case of an error and `e` will contain the reason,
 * If 0, then the `thing` was already in the set, and 1 if it was added.
 */
int ti_vset_add_val(ti_vset_t * vset, ti_val_t * val, ex_t * e)
{
    if (!ti_val_is_thing(val))
    {
        ex_set(e, EX_TYPE_ERROR, "cannot add type `%s` to a "TI_VAL_SET_S,
                ti_val_str(val));
        return e->nr;
    }

    switch((imap_err_t) ti_vset_add(vset, (ti_thing_t *) val))
    {
    case IMAP_ERR_EXIST:
        return 0;
    case IMAP_ERR_ALLOC:
        ex_set_mem(e);
        return e->nr;
    case IMAP_SUCCESS:
        ti_incref(val);
    }
    return 1;
}
