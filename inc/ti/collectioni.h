/*
 * ti/collectioni.h
 */
#ifndef TI_COLLECTIONI_H_
#define TI_COLLECTIONI_H_

#include <qpack.h>
#include <ti/collection.h>
#include <ti/thing.h>

static inline int ti_collection_to_packer(
        ti_collection_t * collection,
        qp_packer_t ** packer)
{
    return (
        qp_add_map(packer) ||
        qp_add_raw_from_str(*packer, "collection_id") ||
        qp_add_int(*packer, collection->root->id) ||
        qp_add_raw_from_str(*packer, "name") ||
        qp_add_raw(*packer, collection->name->data, collection->name->n) ||
        qp_add_raw_from_str(*packer, "things") ||
        qp_add_int(*packer, collection->things->n) ||
        qp_add_raw_from_str(*packer, "quota_things") ||
        ti_quota_val_to_packer(*packer, collection->quota->max_things) ||
        qp_add_raw_from_str(*packer, "quota_properties") ||
        ti_quota_val_to_packer(*packer, collection->quota->max_props) ||
        qp_add_raw_from_str(*packer, "quota_array_size") ||
        ti_quota_val_to_packer(*packer, collection->quota->max_array_size) ||
        qp_add_raw_from_str(*packer, "quota_raw_size") ||
        ti_quota_val_to_packer(*packer, collection->quota->max_raw_size) ||
        qp_close_map(*packer)
    );
}

#endif  /* TI_COLLECTIONI_H_ */
