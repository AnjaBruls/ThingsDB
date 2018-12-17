/*
 * ti/proto.h
 */
#ifndef TI_PROTO_H_
#define TI_PROTO_H_

/*
 * TODO: make tests for checking if `node-responses-128` still map to client
 *       requests. this is required when we forward a query, and alter the
 *       response when returning data to the client.
 */

typedef enum
{
    /*
     * protocol definition for client connections
     */

    /*
     * 0..15 fire and forgets from client to node
     */

    /*
     * 16..32 fire and forgets from node to client
     */
    TI_PROTO_CLIENT_WATCH_INI   =16,    /* {event:x, thing: {#:x, ...}      */
    TI_PROTO_CLIENT_WATCH_UPD   =17,    /* {event:x. #:x, jobs:[] etc }     */
    TI_PROTO_CLIENT_WATCH_DEL   =18,    /* {#:x}                            */
    TI_PROTO_CLIENT_NODE_STATUS =19,    /* str_status                       */

    /*
     * 32..63 client requests
     */
    TI_PROTO_CLIENT_REQ_PING    =32,    /* empty                            */
    TI_PROTO_CLIENT_REQ_AUTH    =33,    /* [username, password]             */
    TI_PROTO_CLIENT_REQ_QUERY   =34,    /* {target:.. query:.. blobs: []}   */
    TI_PROTO_CLIENT_REQ_WATCH   =48,    /* {target:.. things: []}           */
    TI_PROTO_CLIENT_REQ_UNWATCH =49,    /* {target:.. things: []}           */

    /*
     * 64..95 client responses
     */
    TI_PROTO_CLIENT_RES_PING    =64,    /* empty */
    TI_PROTO_CLIENT_RES_AUTH    =65,    /* empty */
    TI_PROTO_CLIENT_RES_QUERY   =66,    /* [{}, {}, ...] */
    TI_PROTO_CLIENT_RES_WATCH   =80,    /* empty */
    TI_PROTO_CLIENT_RES_UNWATCH =81,    /* empty */

    /*
     * 96..127 client errors
     */

    /* zero division error */
    TI_PROTO_CLIENT_ERR_ZERO_DIV    =96,
    /* max quota is reached */
    TI_PROTO_CLIENT_ERR_MAX_QUOTA   =97,
    /* authentication failed or request without authentication */
    TI_PROTO_CLIENT_ERR_AUTH        =98,
    /* no access for the requested task */
    TI_PROTO_CLIENT_ERR_FORBIDDEN   =99,
    /* query syntax error */
    TI_PROTO_CLIENT_ERR_INDEX       =100,
    /* invalid request, incorrect package type, invalid QPack data */
    TI_PROTO_CLIENT_ERR_BAD_REQUEST =101,
    /* node is (currently) unable to respond to the request */
    TI_PROTO_CLIENT_ERR_QUERY       =102,
    /* not found, maybe because due to no access */
    TI_PROTO_CLIENT_ERR_NODE        =103,
    /* internal server error, for example allocation error */
    TI_PROTO_CLIENT_ERR_INTERNAL    =104,


    /*
     * protocol definition for node connections
     */

    /*
     * 128..159 node fire and forgets
     */
    TI_PROTO_NODE_EVENT         =158,   /* event */
    TI_PROTO_NODE_INFO          =159,   /* [...] */

    /*
     * 160..191 node requests
     */

    /* expects a client response which will be forwarded back to the client */
    TI_PROTO_NODE_REQ_QUERY     =162,   /* [user_id, {query...}] */
    /* expects a client response which will be forwarded back to the client */
    TI_PROTO_NODE_REQ_WATCH_ID  =163,   /* thing id */

    TI_PROTO_NODE_REQ_CONNECT   =176,   /* [...] */
    TI_PROTO_NODE_REQ_EVENT_ID  =177,   /* event id */
    TI_PROTO_NODE_REQ_AWAY_ID   =178,   /* away id  */
    TI_PROTO_NODE_REQ_SETUP     =180,   /* empty */

    /*
     * 192..223 node responses
     */
    TI_PROTO_NODE_RES_CONNECT   =208,   /* [node_id, status] */
    TI_PROTO_NODE_RES_EVENT_ID  =209,   /* empty, event id accepted */
    TI_PROTO_NODE_RES_AWAY_ID   =210,   /* empty, away id accepted */
    TI_PROTO_NODE_RES_SETUP     =212,   /* ti_data */

    /*
     * 224..255 node errors
     */
    TI_PROTO_NODE_ERR_RES           =240,   /* message */
    TI_PROTO_NODE_ERR_EVENT_ID      =241,   /* uint8_t node_id with the
                                               conflicting event id */
    TI_PROTO_NODE_ERR_AWAY_ID       =242,   /* empty */

} ti_proto_e;

#define TI_PROTO_NODE_REQ_QUERY_TIMEOUT 120
#define TI_PROTO_NODE_REQ_CONNECT_TIMEOUT 5
#define TI_PROTO_NODE_REQ_EVENT_ID_TIMEOUT 60
#define TI_PROTO_NODE_REQ_AWAY_ID_TIMEOUT 5
#define TI_PROTO_NODE_REQ_SETUP_TIMEOUT 10

const char * ti_proto_str(ti_proto_e tp);

#endif /* TI_PROTO_H_ */
