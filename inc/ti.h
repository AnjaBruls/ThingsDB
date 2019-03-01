/*
 * ti.h
 */
#ifndef TI_H_
#define TI_H_

#include <assert.h>
#include <tiinc.h>
#include <cleri/cleri.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <ti/archive.h>
#include <ti/args.h>
#include <ti/away.h>
#include <ti/build.h>
#include <ti/sync.h>
#include <ti/cfg.h>
#include <ti/clients.h>
#include <ti/collections.h>
#include <ti/connect.h>
#include <ti/counters.h>
#include <ti/events.h>
#include <ti/lookup.h>
#include <ti/node.h>
#include <ti/nodes.h>
#include <ti/store.h>
#include <ti/tcp.h>
#include <ti/val.h>
#include <ti/users.h>
#include <unistd.h>
#include <util/logger.h>
#include <util/smap.h>
#include <util/vec.h>
#include <uv.h>
#include <qpack.h>

typedef struct ti_s ti_t;

extern ti_t ti_;

#define ti_term(signum__) do {\
    if (signum__ != SIGINT) log_critical("raise at: %s:%d,%s (%s)", \
    __FILE__, __LINE__, __func__, strsignal(signum__)); \
    raise(signum__);} while(0)

int ti_create(void);
void ti_destroy(void);
int ti_init_logger(void);
int ti_init(void);
int ti_build(void);
int ti_write_node_id(uint8_t * node_id);
int ti_read_node_id(uint8_t * node_id);
int ti_read(void);
int ti_unpack(uchar * data, size_t n);
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
void ti_set_and_broadcast_node_zone(uint8_t zone);
void ti_broadcast_node_info(void);
int ti_node_to_packer(qp_packer_t ** packer);
ti_val_t * ti_node_as_qpval(void);
static inline ti_t * ti(void);
static inline uint64_t ti_next_thing_id(void);
static inline int ti_sleep(int ms);
static inline const char * ti_name(void);
static inline int ti_to_packer(qp_packer_t ** packer);

struct ti_s
{
    struct timespec boottime;
    char * fn;
    char * node_fn;
    ti_archive_t * archive;     /* committed events archive */
    ti_args_t * args;
    ti_away_t * away;
    ti_build_t * build;         /* only when using --secret */
    ti_cfg_t * cfg;
    ti_clients_t * clients;
    ti_collections_t * collections;
    ti_connect_t * connect_loop;
    ti_counters_t * counters;   /* counters for statistics */
    ti_events_t * events;
    ti_lookup_t * lookup;
    ti_node_t * node;
    ti_nodes_t * nodes;
    ti_store_t * store;
    ti_sync_t * sync;
    ti_thing_t * thing0;        /* thing with id 0 */
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

static inline int ti_to_packer(qp_packer_t ** packer)
{
    return -(
        qp_add_map(packer) ||
        qp_add_raw_from_str(*packer, "schema") ||
        qp_add_int(*packer, TI_FN_SCHEMA) ||
        qp_add_raw_from_str(*packer, "nodes") ||
        ti_nodes_to_packer(packer) ||
        qp_close_map(*packer)
    );
}

#endif /* TI_H_ */
