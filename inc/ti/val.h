/*
 * val.h
 */
#ifndef TI_VAL_H_
#define TI_VAL_H_

typedef enum
{
    TI_VAL_UNDEFINED,
    TI_VAL_NIL,
    TI_VAL_INT,
    TI_VAL_FLOAT,
    TI_VAL_BOOL,
    TI_VAL_RAW,
    TI_VAL_PRIMITIVES,
    TI_VAL_THING,
    TI_VAL_THINGS,
} ti_val_enum;
typedef struct ti_val_s ti_val_t;
typedef union ti_val_u ti_val_via_t;

#include <qpack.h>
#include <stdint.h>
#include <ti/raw.h>
#include <ti/thing.h>
#include <util/vec.h>

ti_val_t * ti_val_create(ti_val_enum tp, void * v);
ti_val_t * ti_val_weak_create(ti_val_enum tp, void * v);
void ti_val_destroy(ti_val_t * val);
void ti_val_weak_set(ti_val_t * val, ti_val_enum tp, void * v);
int ti_val_set(ti_val_t * val, ti_val_enum tp, void * v);
void ti_val_clear(ti_val_t * val);
int ti_val_to_packer(ti_val_t * val, qp_packer_t ** packer);
int ti_val_to_file(ti_val_t * val, FILE * f);
const char * ti_val_to_str(ti_val_t * val);
static inline _Bool ti_val_is_indexable(ti_val_t * val);

union ti_val_u
{
    void * undefined;
    void * nil;
    int64_t int_;
    double float_;
    _Bool bool_;
    ti_raw_t * raw;
    vec_t * primitives;     /* ti_val_t*        */
    ti_thing_t * thing;
    vec_t * things;         /* ti_thing_t*      */
    vec_t * arr;            /* placeholder for primitives and things */
};

struct ti_val_s
{
    ti_val_enum tp;
    ti_val_via_t via;
};

static inline _Bool ti_val_is_indexable(ti_val_t * val)
{
    return (
        val->tp == TI_VAL_RAW ||
        val->tp == TI_VAL_PRIMITIVES ||
        val->tp == TI_VAL_THINGS
    );
}

#endif /* TI_VAL_H_ */
