/*
 * ti/thing.h
 */
#ifndef TI_THING_H_
#define TI_THING_H_

typedef struct ti_thing_s  ti_thing_t;

#include <assert.h>
#include <qpack.h>
#include <stdint.h>
#include <ti/name.h>
#include <ti/val.h>
#include <ti/raw.h>
#include <ti/prop.h>
#include <ti/watch.h>
#include <ti/collection.h>
#include <ti/stream.h>
#include <ti/spec.h>
#include <util/vec.h>
#include <util/imap.h>

ti_thing_t * ti_thing_create(uint64_t id, ti_collection_t * collection);
void ti_thing_destroy(ti_thing_t * thing);
void ti_thing_clear(ti_thing_t * thing);
int ti_thing_props_from_unp(
        ti_thing_t * thing,
        ti_collection_t * collection,
        qp_unpacker_t * unp,
        ssize_t sz,
        ex_t * e);
ti_thing_t * ti_thing_new_from_unp(
        qp_unpacker_t * unp,
        ti_collection_t * collection,
        ssize_t sz,
        ex_t * e);
ti_prop_t * ti_thing_o_prop_add(
        ti_thing_t * thing,
        ti_name_t * name,
        ti_val_t * val);
ti_prop_t * ti_thing_o_prop_set(
        ti_thing_t * thing,
        ti_name_t * name,
        ti_val_t * val);
ti_prop_t * ti_thing_o_prop_set_e(
        ti_thing_t * thing,
        ti_name_t * name,
        ti_val_t * val,
        ex_t * e);
void ti_thing_t_to_object(ti_thing_t * thing);
_Bool ti_thing_o_del(ti_thing_t * thing, ti_name_t * name);
int ti_thing_o_del_e(ti_thing_t * thing, ti_raw_t * rname, ex_t * e);
ti_prop_t * ti_thing_o_weak_get(ti_thing_t * thing, ti_raw_t * r);
ti_prop_t * ti_thing_o_weak_get_e(ti_thing_t * thing, ti_raw_t * r, ex_t * e);
_Bool ti_thing_unset(ti_thing_t * thing, ti_name_t * name);
_Bool ti_thing_rename(ti_thing_t * thing, ti_name_t * from, ti_name_t * to);
int ti_thing_gen_id(ti_thing_t * thing);
ti_watch_t * ti_thing_watch(ti_thing_t * thing, ti_stream_t * stream);
_Bool ti_thing_unwatch(ti_thing_t * thing, ti_stream_t * stream);
int ti_thing__to_packer(ti_thing_t * thing, qp_packer_t ** pckr, int options);
int ti_thing_t_to_packer(ti_thing_t * thing, qp_packer_t ** pckr, int options);
_Bool ti__thing_has_watchers_(ti_thing_t * thing);

struct ti_thing_s
{
    uint32_t ref;
    uint8_t tp;
    uint8_t flags;
    uint16_t type_id;

    uint64_t id;
    ti_collection_t * collection;   /* thing belongs to this collection;
                                     * only `null` when in thingsdb or node
                                     * scope, but never in a collection scope
                                     */
    vec_t * items;                  /* vec contains ti_prop_t */
    vec_t * watchers;               /* vec contains ti_watch_t,
                                       NULL if no watchers,  */
};

static inline _Bool ti_thing_is_object(ti_thing_t * thing)
{
    return thing->type_id == TI_SPEC_OBJECT;
}

static inline int ti_thing_to_packer(
        ti_thing_t * thing,
        qp_packer_t ** pckr,
        int options)
{
    return options > 0 || ti_thing_is_object(thing)
            ? ti_thing__to_packer(thing, pckr, options)
            : ti_thing_t_to_packer(thing, pckr, options);
}

static inline _Bool ti_thing_has_watchers(ti_thing_t * thing)
{
    return thing->watchers && ti__thing_has_watchers_(thing);
}

static inline int ti_thing_id_to_packer(
        ti_thing_t * thing,
        qp_packer_t ** packer)
{
    return (qp_add_map(packer) ||
            (thing->id && (
                    qp_add_raw(*packer, (const uchar *) TI_KIND_S_THING, 1) ||
                    qp_add_int(*packer, thing->id))) ||
            qp_close_map(*packer));
}

static inline int ti_thing_id_to_file(ti_thing_t * thing, FILE * f)
{
    return (
            qp_fadd_type(f, QP_MAP1) ||
            qp_fadd_raw(f, (const uchar *) TI_KIND_S_THING, 1) ||
            qp_fadd_int(f, thing->id)
    );
}

static inline _Bool ti_thing_is_new(ti_thing_t * thing)
{
    return thing->flags & TI_VFLAG_THING_NEW;
}
static inline void ti_thing_mark_new(ti_thing_t * thing)
{
    thing->flags |= TI_VFLAG_THING_NEW;
}
static inline void ti_thing_unmark_new(ti_thing_t * thing)
{
    thing->flags &= ~TI_VFLAG_THING_NEW;
}
static inline uint64_t ti_thing_key(ti_thing_t * thing)
{
    return (uintptr_t) thing;
}

static inline ti_val_t * ti_thing_o_val_weak_get(
        ti_thing_t * thing,
        ti_name_t * name)
{
    assert (ti_thing_is_object(thing));
    for (vec_each(thing->items, ti_prop_t, prop))
        if (prop->name == name)
            return prop->val;
    return NULL;
}

static inline ti_prop_t * ti_thing_o_prop_weak_get(
        ti_thing_t * thing,
        ti_name_t * name)
{
    assert (ti_thing_is_object(thing));
    for (vec_each(thing->items, ti_prop_t, prop))
        if (prop->name == name)
            return prop;
    return NULL;
}

#define thing_each(t__, name__, val__)                          \
    void ** v__ = t__->items->data,                             \
    ** e__ = v__ + t__->items->n,                               \
    ** n__ = ti_thing_type(t__)->fields->data;                  \
    v__ < e__ && (((name__ = *n__) && (val__ = *v__)) || 1);     \
    ++v__, ++n__

#endif /* TI_THING_H_ */
