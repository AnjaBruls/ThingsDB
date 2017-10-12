/*
 * vec.c
 *
 *  Created on: Sep 30, 2017
 *      Author: Jeroen van der Heijden <jeroen@transceptor.technology>
 */
#include <string.h>
#include <stdlib.h>
#include <util/vec.h>

/*
 * Returns a new vec with size sz.
 */
vec_t * vec_new(uint32_t sz)
{
    vec_t * vec = (vec_t *) malloc(sizeof(vec_t) + sz * sizeof(void*));
    if (!vec) return NULL;
    vec->sz = sz;
    vec->n = 0;
    return vec;
}

/*
 * Destroy a vec with optional callback. If callback is NULL then it is
 * just as safe to simply call free() instead of this function.
 */
void vec_destroy(vec_t * vec, vec_destroy_cb cb)
{
    if (vec && cb)
    {
        for (vec_each(vec, void, obj), (*cb)(obj));
    }
    free(vec);
}

/*
 * Returns a copy of vec with an exact fit so the new vec->sz and vec->n will
 * be equal. In case of an allocation error the return value is NULL.
 */
vec_t * vec_dup(vec_t * vec)
{
    size_t sz = sizeof(vec_t) + vec->n * sizeof(void*);
    vec_t * v = (vec_t *) malloc(sz);
    if (!v) return NULL;
    memcpy(v, vec, sz);
    v->sz = v->n;
    return v;
}

/*
 * Append data to vec and returns vec.
 *
 * Returns a pointer to vec. The returned vec can be equal to the original
 * vec but there is no guarantee. The return value is NULL in case of an
 * allocation error.
 */
vec_t * vec_push(vec_t * vec, void * data)
{
    if (vec->n == vec->sz)
    {
        size_t sz = vec->sz;

        if (sz < 4)
        {
            vec->sz++;
        }
        else if (sz < 64)
        {
            vec->sz *= 2;
        }
        else
        {
            vec->sz += 64;
        }

        vec_t * tmp = (vec_t *) realloc(
                vec,
                sizeof(vec_t) + vec->sz * sizeof(void*));

        if (!tmp)
        {
            /* restore original size */
            vec->sz = sz;
            return NULL;
        }

        vec = tmp;
    }
    VEC_push(vec, data);
    return vec;
}

/*
 * Extends a vec with n data elements and returns the new extended vec.
 *
 * In case of an error NULL is returned.
 */
vec_t * vec_extend(vec_t * vec, void * data[], uint32_t n)
{
    vec->n += n;
    if (vec->n > vec->sz)
    {
        vec_t * tmp = (vec_t *) realloc(
                vec,
                sizeof(vec_t) + vec->n * sizeof(void*));
        if (!tmp)
        {
            /* restore original length */
            vec->n -= n;
            return NULL;
        }

        vec = tmp;
        vec->sz = vec->n;
    }
    memcpy(vec->data + (vec->n - n), data, n * sizeof(void*));
    return vec;
}

/*
 * Resize a vec to sz. If the new size is smaller then vec->n might decrease.
 */
vec_t * vec_resize(vec_t * vec, uint32_t sz)
{
    vec_t * v = (vec_t *) realloc(
            vec,
            sizeof(vec_t) + sz * sizeof(void*));
    if (!v) return NULL;
    if (v->n > sz)
    {
        v->n = sz;
    }
    v->sz = sz;
    return v;
}

/*
 * Shrinks a vec to an exact fit.
 *
 * Returns a pointer to the new vec.
 */
vec_t * vec_shrink(vec_t * vec)
{
    if (vec->n == vec->sz) return vec;
    vec_t * v = (vec_t *) realloc(
            vec,
            sizeof(vec_t) + vec->n * sizeof(void*));
    if (!v) return NULL;
    v->sz = v->n;
    return v;
}


