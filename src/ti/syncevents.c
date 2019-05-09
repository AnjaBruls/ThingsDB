/*
 * ti/syncevents.c
 */
#include <assert.h>
#include <ti/proto.h>
#include <ti.h>
#include <ti/nodes.h>
#include <ti/syncevents.h>
#include <util/qpx.h>
#include <util/syncpart.h>

static void syncevents__push_cb(ti_req_t * req, ex_enum status);
static void syncevents__done_cb(ti_req_t * req, ex_enum status);


static int syncevents__send(ti_stream_t * stream, ti_epkg_t * epkg)
{
    ti_pkg_t * pkg = ti_pkg_dup(epkg->pkg);
    if (!pkg)
        return -1;

    pkg->id = 0;
    ti_pkg_set_tp(pkg, TI_PROTO_NODE_REQ_SYNCEPART);

    if (ti_req_create(
            stream,
            pkg,
            TI_PROTO_NODE_REQ_SYNCEPART_TIMEOUT,
            syncevents__push_cb,
            NULL))
    {
        free(pkg);
        return -1;
    }
    return 0;
}

/*
 * Returns 1 if the given `event_id` is not found and 0 if it is found
 * and a request is successfully made. In case of an error, -1
 * will be the return value.
 */
int ti_syncevents_init(ti_stream_t * stream, uint64_t event_id)
{
    queue_t * events_queue = ti()->archive->queue;

    for (queue_each(events_queue, ti_epkg_t, epkg))
    {
        if (epkg->event_id == event_id)
        {
            return syncevents__send(stream, epkg);
        }
    }

    events_queue = ti()->events->queue;
    return 1;
}

ti_pkg_t * ti_syncevents_on_part(ti_pkg_t * pkg, ex_t * e)
{
    int rc;
    ti_pkg_t * resp;
    qpx_packer_t * packer = qpx_packer_create(9, 1);
    ti_epkg_t * epkg = ti_epkg_from_pkg(pkg);
    uint64_t next_event_id;

    if (!packer || !epkg)
    {
        ex_set_alloc(e);  /* can leak a few bytes */
        return NULL;
    }

    next_event_id = epkg->event_id + 1;

    rc = ti_events_add_event(ti()->node, epkg);
    ti_epkg_drop(epkg);

    if (rc < 0)
    {
        ex_set_internal(e);  /* can leak a few bytes */
        return NULL;
    }

    qp_add_int(packer, next_event_id);

    resp = qpx_packer_pkg(packer, TI_PROTO_NODE_RES_SYNCEPART);
    resp->id = pkg->id;

    return resp;
}

int ti_syncevents_done(ti_stream_t * stream)
{
    ti_pkg_t * pkg = ti_pkg_new(0, TI_PROTO_NODE_REQ_SYNCEDONE, NULL, 0);

    if (!pkg)
        return -1;

    if (ti_req_create(
            stream,
            pkg,
            TI_PROTO_NODE_REQ_SYNCEDONE_TIMEOUT,
            syncevents__done_cb,
            NULL))
    {
        free(pkg);
        return -1;
    }
    return 0;
}

static void syncevents__push_cb(ti_req_t * req, ex_enum status)
{
    qp_unpacker_t unpacker;
    ti_pkg_t * pkg = req->pkg_res;
    qp_obj_t qp_event_id;
    uint64_t next_event_id;
    int rc;

    if (status)
        goto failed;

    if (!req->stream)
    {
        log_error("connection to stream lost while synchronizing");
        goto failed;
    }

    if (pkg->tp != TI_PROTO_NODE_RES_SYNCEPART)
    {
        ti_pkg_log(pkg);
        goto failed;
    }

    qp_unpacker_init2(&unpacker, pkg->data, pkg->n, 0);

    if (!qp_is_int(qp_next(&unpacker, &qp_event_id)))
    {
        log_error("invalid `%s`", ti_proto_str(pkg->tp));
        goto failed;
    }

    next_event_id = (uint64_t) qp_event_id.via.int64;

    rc = ti_syncevents_init(req->stream, next_event_id);
    if (rc < 0)
    {
        log_error(
                "failed creating request for stream `%s` and "TI_EVENT_ID,
                ti_stream_name(req->stream),
                next_event_id);
        goto failed;
    }

    if (rc > 0 && ti_syncevents_done(req->stream))
        goto failed;

    goto done;

failed:
    ti_stream_stop_watching(req->stream);
done:
    ti_req_destroy(req);
}

static void syncevents__done_cb(ti_req_t * req, ex_enum status)
{
    log_info("finished synchronizing `%s`", ti_stream_name(req->stream));

    if (status)
        log_error("failed response: `%s` (%s)", ex_str(status), status);

    ti_away_syncer_done(req->stream);
    ti_stream_stop_watching(req->stream);

    ti_req_destroy(req);
}


