/*
 * ti/varr.c
 */
#include <assert.h>
#include <tiinc.h>
#include <stdlib.h>
#include <ti/varr.h>
#include <ti/val.h>
#include <ti/vset.h>
#include <ti/opr.h>
#include <ti/closure.h>
#include <util/logger.h>

static int varr__to_tuple(ti_varr_t ** varr)
{
    ti_varr_t * tuple = *varr;

    if (tuple->flags & TI_VFLAG_ARR_TUPLE)
        return 0;  /* tuples cannot change so we do not require a copy */

    if (tuple->ref == 1)
    {
        tuple->flags |= TI_VFLAG_ARR_TUPLE;
        return 0;  /* with only one reference we do not require a copy */
    }

    tuple = malloc(sizeof(ti_varr_t));
    if (!tuple)
        return -1;

    tuple->ref = 1;
    tuple->tp = TI_VAL_ARR;
    tuple->flags = TI_VFLAG_ARR_TUPLE | ((*varr)->flags & TI_VFLAG_ARR_MHT);
    tuple->vec = vec_dup((*varr)->vec);

    if (!tuple->vec)
    {
        free(tuple);
        return -1;
    }

    for (vec_each(tuple->vec, ti_val_t, val))
        ti_incref(val);

    assert ((*varr)->ref > 1);
    ti_decref(*varr);
    *varr = tuple;
    return 0;
}

ti_varr_t * ti_varr_create(size_t sz)
{
    ti_varr_t * varr = malloc(sizeof(ti_varr_t));
    if (!varr)
        return NULL;

    varr->ref = 1;
    varr->tp = TI_VAL_ARR;
    varr->flags = TI_VFLAG_UNASSIGNED;

    varr->vec = vec_new(sz);
    if (!varr->vec)
    {
        free(varr);
        return NULL;
    }

    return varr;
}

ti_varr_t * ti_varr_from_vec(vec_t * vec)
{
    ti_varr_t * varr = malloc(sizeof(ti_varr_t));
    if (!varr)
        return NULL;

    varr->ref = 1;
    varr->tp = TI_VAL_ARR;
    varr->flags = TI_VFLAG_UNASSIGNED;

    varr->vec = vec;
    return varr;
}

void ti_varr_destroy(ti_varr_t * varr)
{
    if (!varr)
        return;
    vec_destroy(varr->vec, (vec_destroy_cb) ti_val_drop);
    free(varr);
}

/*
 * does not increment `*v` reference counter but the value might change to
 * a (new) tuple pointer.
 */
int ti_varr_append(ti_varr_t * to, void ** v, ex_t * e)
{
    assert (ti_varr_is_list(to));  /* `to` must be a list */

    switch (((ti_val_t *) *v)->tp)
    {
    case TI_VAL_QP:
        /* can only be added to unassigned arrays and since the `QP` type is
         * only used at root, and arrays cannot be assigned at root, the `to`
         * array must be an unassigned array */
        assert (!ti_varr_is_assigned(to));
        break;
    case TI_VAL_SET:
        if (ti_vset_to_tuple((ti_vset_t **) v))
        {
            ex_set_alloc(e);
            return e->nr;
        }
        to->flags |= ((ti_varr_t *) *v)->flags & TI_VFLAG_ARR_MHT;
        break;
    case TI_VAL_CLOSURE:
        if (ti_closure_wse((ti_closure_t *) *v))
        {
            ex_set(e, EX_BAD_DATA,
                "closures with side effects cannot be assigned");
            return e->nr;
        }
        if (ti_closure_unbound((ti_closure_t *) *v, e))
        {
            ex_set_alloc(e);
            return e->nr;
        }
        break;
    case TI_VAL_ARR:
        if (ti_varr_is_list((ti_varr_t *) *v))
        {
            if (varr__to_tuple((ti_varr_t **) v))
            {
                ex_set_alloc(e);
                return e->nr;
            }
        }
        to->flags |= ((ti_varr_t *) *v)->flags & TI_VFLAG_ARR_MHT;
        break;
    case TI_VAL_THING:
        to->flags |= TI_VFLAG_ARR_MHT;
        break;
    }

    if (vec_push(&to->vec, *v))
        ex_set_alloc(e);

    return e->nr;
}

_Bool ti_varr_has_things(ti_varr_t * varr)
{
    if (ti_varr_may_have_things(varr))
    {
        for (vec_each(varr->vec, ti_val_t, val))
            if (val->tp == TI_VAL_THING)
                return true;

        /* Remove the flag since no `things` are found in the array */
        varr->flags &= ~TI_VFLAG_ARR_MHT;
    }
    return false;
}

int ti_varr_to_list(ti_varr_t ** varr)
{
    ti_varr_t * list = *varr;

    if (list->ref == 1)
    {
        /* This can never happen to a tuple since a tuple is always nested
         * and therefore always has more than one reference */
        assert (~list->flags & TI_VFLAG_ARR_TUPLE);
        ti_varr_set_assigned(list);
        return 0;
    }

    list = malloc(sizeof(ti_varr_t));
    if (!list)
        return -1;

    list->ref = 1;
    list->tp = TI_VAL_ARR;
    list->flags = (*varr)->flags & TI_VFLAG_ARR_MHT;
    list->vec = vec_dup((*varr)->vec);

    if (!list->vec)
    {
        free(list);
        return -1;
    }

    for (vec_each(list->vec, ti_val_t, val))
        ti_incref(val);

    ti_decref(*varr);
    *varr = list;

    return 0;
}

/*
 * Do not use this method, but the in-line method ti_varr_eq() instead
 */
_Bool ti__varr_eq(ti_varr_t * varra, ti_varr_t * varrb)
{
    size_t i = 0;

    assert (varra != varrb && varra->vec->n == varrb->vec->n);
    for (vec_each(varra->vec, ti_val_t, va), ++i)
        if (!ti_opr_eq(va, vec_get(varrb->vec, i)))
            return false;
    return true;
}

