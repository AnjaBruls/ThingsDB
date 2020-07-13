/*
 * ti/key.t.h
 */
#ifndef TI_KEY_T_H_
#define TI_KEY_T_H_

/* it is possible to extend the key limit without consequences */
#define TI_KEY_MAX 255

typedef struct ti_key_s ti_key_t;

#include <inttypes.h>

/*
 * key can be cast to raw. the difference is in `char str[]`, but the other
 * fields map to `raw`. therefore the `tp` of key should be equal to `raw`
 */
struct ti_key_s
{
    uint32_t ref;
    uint8_t tp;
    uint8_t _flags;
    uint16_t _pad0;
    uint32_t n;
    char str[];             /* NOT null terminated */
};

#endif /* TI_KEY_T_H_ */
