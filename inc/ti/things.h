/*
 * ti/things.h
 */
#ifndef TI_THINGS_H_
#define TI_THINGS_H_

#include <qpack.h>
#include <stdint.h>
#include <ti/thing.h>
#include <ti/collection.h>
#include <util/imap.h>

ti_thing_t * ti_things_create_thing_o(
        uint64_t id,
        ti_collection_t * collection);
ti_thing_t * ti_things_create_thing_t(
        uint64_t id,
        ti_type_t * type,
        ti_collection_t * collection);
ti_thing_t * ti_things_thing_o_from_unp(
        ti_collection_t * collection,
        uint64_t thing_id,
        qp_unpacker_t * unp,
        ssize_t sz,
        ex_t * e);
ti_thing_t * ti_things_thing_t_from_unp(
        ti_collection_t * collection,
        qp_unpacker_t * unp,
        ex_t * e);
int ti_things_gc(imap_t * things, ti_thing_t * root);


#endif /* TI_THINGS_H_ */
