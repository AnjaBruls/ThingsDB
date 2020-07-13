/*
 * ti/key.inline.h
 */
#ifndef TI_KEY_INLINE_H_
#define TI_KEY_INLINE_H_

static inline _Bool ti_key_is_name(ti_key_t * key)
{
    return key->tp == TI_VAL_NAME;
}

#endif /* TI_KEY_INLINE_H_ */
