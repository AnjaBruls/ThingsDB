/*
 * node.h
 *
 *  Created on: Oct 5, 2017
 *      Author: Jeroen van der Heijden <jeroen@transceptor.technology>
 */
#ifndef RQL_NODE_H_
#define RQL_NODE_H_

typedef enum
{
    RQL_NODE_STAT_OFFLINE,
    RQL_NODE_STAT_CONNECTED,
    RQL_NODE_STAT_MAINTENANCE,
    RQL_NODE_STAT_READY
} rql_node_status_t;

typedef struct rql_node_s  rql_node_t;

#include <inttypes.h>
#include <rql/sock.h>
#include <util/imap.h>

struct rql_node_s
{
    uint64_t ref;
    uint8_t id;  /* equal to the index in rql->nodes */
    uint8_t flags;
    uint8_t status;
    uint8_t pad0;
    uint16_t port;
    uint16_t req_next_id;
    imap_t * reqs;
    rql_sock_t * sock;
    char * addr;    /* can be used as name */
};

rql_node_t * rql_node_create(uint8_t id, char * address, uint16_t port);
rql_node_t * rql_node_grab(rql_node_t * node);
void rql_node_drop(rql_node_t * node);
//int rql_node_write(rql_node_t * node, rql_pkg_t * pkg, rql_write_cb cb);

#endif /* RQL_NODE_H_ */
