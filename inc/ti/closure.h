
/*
 * ti/closure.h
 */
#ifndef TI_CLOSURE_H_
#define TI_CLOSURE_H_

#include <cleri/cleri.h>
#include <ex.h>
#include <stdint.h>
#include <ti/closure.t.h>
#include <ti/name.t.h>
#include <ti/prop.t.h>
#include <ti/qbind.t.h>
#include <ti/query.t.h>
#include <ti/val.t.h>
#include <util/mpack.h>

ti_closure_t * ti_closure_from_node(cleri_node_t * node, uint8_t flags);
ti_closure_t * ti_closure_from_strn(
        ti_qbind_t * syntax,
        const char * str,
        size_t n, ex_t * e);
void ti_closure_destroy(ti_closure_t * closure);
int ti_closure_unbound(ti_closure_t * closure, ex_t * e);
int ti_closure_to_pk(ti_closure_t * closure, msgpack_packer * pk);
char * ti_closure_char(ti_closure_t * closure, size_t * n);
int ti_closure_inc(ti_closure_t * closure, ti_query_t * query, ex_t * e);
void ti_closure_dec(ti_closure_t * closure, ti_query_t * query);
int ti_closure_vars_nameval(
        ti_closure_t * closure,
        ti_name_t * name,
        ti_val_t * val,
        ex_t * e);
int ti_closure_vars_val_idx(ti_closure_t * closure, ti_val_t * v, int64_t i);
int ti_closure_vars_vset(ti_closure_t * closure, ti_thing_t * t);
int ti_closure_call(
        ti_closure_t * closure,
        ti_query_t * query,
        vec_t * args,
        ex_t * e);
ti_raw_t * ti_closure_doc(ti_closure_t * closure);
ti_raw_t * ti_closure_def(ti_closure_t * closure);

static inline int ti_closure_vars_prop(
        ti_closure_t * closure,
        ti_prop_t * prop,
        ex_t * e)
{
    return ti_closure_vars_nameval(closure, prop->name, prop->val, e);
}

static inline cleri_node_t * ti_closure_statement(ti_closure_t * closure)
{
    /*  closure = Sequence('|', List(name, opt=True), '|', statement)  */
    return closure->node->children->next->next->next->node;
}

#endif  /* TI_CLOSURE_H_ */
