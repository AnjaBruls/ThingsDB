/*
 * ti.h
 */
#ifndef TI_H_
#define TI_H_

#define TI_URL "https://thinkdb.net"
#define TI_DOCS TI_URL"/docs/"
#define TI_THING_ID "`#%"PRIu64"`"
#define TI_EVENT_ID "`event:%"PRIu64"`"
#define TI_COLLECTION_ID "`collection:%"PRIu64"`"
#define TI_USER_ID "`user:%"PRIu64"`"

typedef unsigned char uchar;

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#define TI_CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW

#define ti_grab(x) ((x) && ++(x)->ref ? (x) : NULL)
#define ti_incref(x) (++(x)->ref)
#define ti_decref(x) (--(x)->ref)  /* use only when x->ref > 1 */

/* SUSv2 guarantees that "Host names are limited to 255 bytes,
 * excluding terminating null byte" */
#define TI_MAX_HOSTNAME_SZ 256

enum
{
    TI_FLAG_SIGNAL      =1<<0,
    TI_FLAG_STOP        =1<<1,
    TI_FLAG_INDEXING    =1<<2,
    TI_FLAG_LOCKED      =1<<3,
};

typedef struct ti_s ti_t;

#include <assert.h>
#include <cleri/cleri.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <ti/archive.h>
#include <ti/args.h>
#include <ti/away.h>
#include <ti/cfg.h>
#include <ti/clients.h>
#include <ti/collections.h>
#include <ti/connect.h>
#include <ti/counters.h>
#include <ti/events.h>
#include <ti/desired.h>
#include <ti/lookup.h>
#include <ti/node.h>
#include <ti/nodes.h>
#include <ti/store.h>
#include <ti/tcp.h>
#include <ti/users.h>
#include <unistd.h>
#include <util/logger.h>
#include <util/smap.h>
#include <util/vec.h>
#include <uv.h>

extern ti_t ti_;

#define ti_term(signum__) do {\
    log_critical("raise at: %s:%d,%s (%s)", \
    __FILE__, __LINE__, __func__, strsignal(signum__)); \
    raise(signum__);} while(0)

int ti_create(void);
void ti_destroy(void);
void ti_init_logger(void);
int ti_init(void);
int ti_build(void);
int ti_read(void);
int ti_run(void);
void ti_stop_slow(void);
void ti_stop(void);
int ti_save(void);
int ti_lock(void);
int ti_unlock(void);
ti_rpkg_t * ti_node_status_rpkg(void);  /* returns package with next_thing_id,
                                           cevid, ti_node->status
                                        */
void ti_set_and_broadcast_node_status(ti_node_status_t status);
static inline ti_t * ti(void);
static inline _Bool ti_manages_id(uint64_t id);
static inline uint64_t ti_next_thing_id(void);
static inline int ti_sleep(int ms);
static inline const char * ti_name(void);

struct ti_s
{
    struct timespec boottime;
    char * fn;
    ti_archive_t * archive;     /* committed events archive */
    ti_counters_t * counters;   /* counters for statistics */
    ti_node_t * node;
    ti_args_t * args;
    ti_cfg_t * cfg;
    ti_away_t * away;
    ti_lookup_t * lookup;
    ti_desired_t * desired;     /* NULL or a desired state lookup */
    ti_clients_t * clients;
    ti_events_t * events;
    ti_nodes_t * nodes;
    ti_store_t * store;
    ti_connect_t * connect_loop;
    ti_thing_t * thing0;        /* thing with id 0 */
    ti_collections_t * collections;
    ti_users_t * users;
    vec_t * access;
    smap_t * names;             /* weak map for ti_name_t */
    uv_loop_t * loop;
    cleri_grammar_t * langdef;
    uint64_t stored_event_id;   /* last stored event id (excluding archive) */
    uint64_t * next_thing_id;   /* pointer to ti->node->next_thing_id used
                                   for assigning id's to objects
                                */
    uint8_t flags;
    char hostname[256];

};

static inline ti_t * ti(void)
{
    return &ti_;
}

/*
 * Returns true if this is `should` be managed by ThingsDB, it can be that
 * the id is still in the desired lookup and attributes are not available yet,
 * if you want to know if the thing has attributes, check ti_thing_with_attrs()
 */
static inline _Bool ti_manages_id(uint64_t id)
{
    return (
        ti_node_manages_id(ti_.node, ti_.lookup, id) ||
        (ti_.desired && ti_node_manages_id(ti_.node, ti_.desired->lookup, id))
    );
}

/* return the next thing id and increment by one */
static inline uint64_t ti_next_thing_id(void)
{
    return (*ti_.next_thing_id)++;
}

/* sleep in milliseconds (value must be between 0 and 999 */
static inline int ti_sleep(int ms)
{
    assert (ms < 1000);
    return (ti_.flags & TI_FLAG_SIGNAL)
            ? -2
            : nanosleep((const struct timespec[]){{0, ms * 1000000L}}, NULL);
}

static inline const char * ti_name(void)
{
    return ti_.hostname;
}

#endif /* TI_H_ */
