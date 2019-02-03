/*
 * ti/val.h
 */
#ifndef TI_VAL_H_
#define TI_VAL_H_

typedef enum
{
    TI_VAL_ATTR,    /* attribute ti_prop_t */
    TI_VAL_NIL,
    TI_VAL_INT,
    TI_VAL_FLOAT,
    TI_VAL_BOOL,
    TI_VAL_QP,      /* QPack data, only used on root for returning raw packed
                       data */
    TI_VAL_RAW,
    TI_VAL_REGEX,
    TI_VAL_ARRAY,   /* NEVER turn back to TI_VAL_THINGS */
    TI_VAL_TUPLE,   /* nested arrays are of tuple type */
    TI_VAL_THING,
    TI_VAL_THINGS,  /* when empty, this can be turned into TI_VAL_ARRAY */
    TI_VAL_ARROW,
} ti_val_enum;

typedef enum
{
    TI_VAL_FLAG_FETCH       =1<<0,
    TI_VAL_FLAG_STATIC      =1<<2,
} ti_val_flags;

typedef enum
{
    TI_VAL_KIND_THING   ='#',
    TI_VAL_KIND_ARROW   ='$',
    TI_VAL_KIND_REGEX   ='*',
} ti_val_kind;

enum
{
    TI_VAL_PACK_NEW     =1<<0,
    TI_VAL_PACK_THING   =1<<1,
    TI_VAL_PACK_ATTR    =1<<2,
};

typedef struct ti_val_s ti_val_t;
typedef union ti_val_u ti_val_via_t;

#include <qpack.h>
#include <cleri/cleri.h>
#include <stdint.h>
#include <ti/raw.h>
#include <ti/thing.h>
#include <ti/name.h>
#include <ti/regex.h>
#include <ti/ex.h>
#include <util/vec.h>
#include <util/imap.h>

ti_val_t * ti_val_create(ti_val_enum tp, void * v);
ti_val_t * ti_val_weak_create(ti_val_enum tp, void * v);
ti_val_t * ti_val_dup(ti_val_t * val);
ti_val_t * ti_val_weak_dup(ti_val_t * val);
int ti_val_from_unp(ti_val_t * dest, qp_unpacker_t * unp, imap_t * things);
void ti_val_destroy(ti_val_t * val);
static inline void ti_val_weak_destroy(ti_val_t * val);
void ti_val_weak_set(ti_val_t * val, ti_val_enum tp, void * v);
int ti_val_set(ti_val_t * val, ti_val_enum tp, void * v);
int ti_val_convert_to_str(ti_val_t * val);
void ti_val_weak_copy(ti_val_t * to, ti_val_t * from);
int ti_val_copy(ti_val_t * to, ti_val_t * from);
void ti_val_set_arrow(ti_val_t * val, cleri_node_t * arrow_nd);
void ti_val_set_bool(ti_val_t * val, _Bool bool_);
void ti_val_set_nil(ti_val_t * val);
void ti_val_set_undefined(ti_val_t * val);
void ti_val_set_int(ti_val_t * val, int64_t i);
void ti_val_set_float(ti_val_t * val, double d);
void ti_val_set_thing(ti_val_t * val, ti_thing_t * thing);
_Bool ti_val_as_bool(ti_val_t * val);
_Bool ti_val_is_valid_name(ti_val_t * val);
size_t ti_val_iterator_n(ti_val_t * val);
int ti_val_gen_ids(ti_val_t * val);
void ti_val_clear(ti_val_t * val);
int ti_val_to_packer(ti_val_t * val, qp_packer_t ** packer, int flags);
int ti_val_to_file(ti_val_t * val, FILE * f);
const char * ti_val_tp_str(ti_val_enum tp);
_Bool ti_val_startswith(ti_val_t * a, ti_val_t * b);
_Bool ti_val_endswith(ti_val_t * a, ti_val_t * b);
int ti_val_move_to_arr(ti_val_t * to_arr, ti_val_t * val, ex_t * e);
int ti_val_check_assignable(ti_val_t * val, _Bool to_array, ex_t * e);
static inline const char * ti_val_str(ti_val_t * val);
static inline _Bool ti_val_is_arr(ti_val_t * val);
static inline _Bool ti_val_is_raw(ti_val_t * val);
static inline _Bool ti_val_is_settable(ti_val_t * val);
static inline _Bool ti_val_is_mutable_arr(ti_val_t * val);
static inline _Bool ti_val_is_indexable(ti_val_t * val);
static inline _Bool ti_val_is_iterable(ti_val_t * val);
static inline _Bool ti_val_is_array(ti_val_t * val);
static inline _Bool ti_val_is_list(ti_val_t * val);
static inline void ti_val_mark_fetch(ti_val_t * val);
static inline void ti_val_unmark_fetch(ti_val_t * val);

union ti_val_u
{
    void * attr;
    cleri_node_t * arrow;
    void * nil;
    int64_t int_;
    double float_;
    _Bool bool_;
    ti_raw_t * qp;
    ti_raw_t * raw;
    ti_regex_t * regex;
    vec_t * array;          /* ti_val_t*        */
    vec_t * tuple;          /* ti_val_t*        */
    ti_thing_t * thing;
    vec_t * things;         /* ti_thing_t*      */
    vec_t * arr;            /* placeholder for array and things */
};

struct ti_val_s
{
    uint8_t tp;
    uint8_t flags;
    ti_val_via_t via;
};

static inline void ti_val_weak_destroy(ti_val_t * val)
{
    free(val);
}

static inline const char * ti_val_str(ti_val_t * val)
{
    return ti_val_tp_str(val->tp);
}

static inline _Bool ti_val_is_arr(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_TUPLE ||
        val->tp == TI_VAL_THINGS
    );
}

static inline _Bool ti_val_is_raw(ti_val_t * val)
{
    return val->tp == TI_VAL_RAW;
}

static inline _Bool ti_val_is_settable(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_INT ||
        val->tp == TI_VAL_FLOAT ||
        val->tp == TI_VAL_BOOL ||
        val->tp == TI_VAL_RAW ||
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_TUPLE);
}

static inline _Bool ti_val_is_mutable_arr(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_THINGS
    );
}

static inline _Bool ti_val_is_indexable(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_RAW ||
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_TUPLE ||
        val->tp == TI_VAL_THINGS
    );
}

static inline _Bool ti_val_is_iterable(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_RAW ||
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_TUPLE ||
        val->tp == TI_VAL_THING ||
        val->tp == TI_VAL_THINGS
    );
}

static inline _Bool ti_val_is_array(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_ARRAY ||
        val->tp == TI_VAL_TUPLE ||
        val->tp == TI_VAL_THINGS
    );
}

static inline _Bool ti_val_is_list(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_ARRAY ||
        (val->tp == TI_VAL_THINGS && !val->via.arr->n)
    );
}

static inline void ti_val_mark_fetch(ti_val_t * val)
{
    val->flags |= TI_VAL_FLAG_FETCH;
}

static inline void ti_val_unmark_fetch(ti_val_t * val)
{
    val->flags &= ~TI_VAL_FLAG_FETCH;
}

#endif /* TI_VAL_H_ */
