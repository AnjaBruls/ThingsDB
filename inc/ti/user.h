/*
 * user.h
 *
 *  Created on: Oct 5, 2017
 *      Author: Jeroen van der Heijden <jeroen@transceptor.technology>
 */
#ifndef TI_USER_H_
#define TI_USER_H_

#define TI_CFG_PATH_MAX 4096
#define TI_CFG_ADDR_MAX 256

typedef struct ti_user_s  ti_user_t;

#include <stdint.h>
#include <ti/raw.h>
#include <util/ex.h>

extern const char * ti_user_def_name;
extern const char * ti_user_def_pass;
extern const unsigned int ti_min_name;  // length including terminator
extern const unsigned int ti_max_name;  // length including terminator
extern const unsigned int ti_min_pass;  // length including terminator
extern const unsigned int ti_max_pass;  // length including terminator

ti_user_t * ti_user_create(
        uint64_t id,
        const ti_raw_t * name,
        const char * encrpass);
ti_user_t * ti_user_grab(ti_user_t * user);
void ti_user_drop(ti_user_t * user);
int ti_user_name_check(const ti_raw_t * name, ex_t * e);
int ti_user_pass_check(const ti_raw_t * pass, ex_t * e);
int ti_user_rename(ti_user_t * user, const ti_raw_t * name);
int ti_user_set_pass(ti_user_t * user, const char * pass);

struct ti_user_s
{
    uint32_t ref;
    uint64_t id;
    ti_raw_t * name;
    char * pass;
};

#endif /* TI_USER_H_ */