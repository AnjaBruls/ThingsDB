/*
 * back.c
 */
#include <stdlib.h>
#include <thingsdb.h>
#include <sys/socket.h>
#include <ti/back.h>
#include <ti/ref.h>
#include <util/logger.h>

static void ti__back_on_connect(uv_tcp_t * tcp, int status);
static void ti__back_on_pkg(ti_sock_t * sock, ti_pkg_t * pkg);

ti_back_t * ti_back_create(void)
{
    ti_back_t * back = malloc(sizeof(ti_back_t));
    if (!back)
        return NULL;

    back->sock = ti_sock_create(TI_SOCK_BACK);
    if (!back->sock)
    {
        ti_back_destroy(back);
        return NULL;
    }

    return back;
}

void ti_back_destroy(ti_back_t * back)
{
    if (!back)
        return;
    ti_sock_drop(back->sock);
    free(back);
}

int ti_back_listen(ti_back_t * back)
{
    int rc;
    ti_cfg_t * cfg = thingsdb_get()->cfg;

    if (ti_sock_init(back->sock)) return -1;
    TI_ref_inc(back->sock);

    struct sockaddr_storage addr;

    if (cfg->ip_support == AF_INET)
    {
        uv_ip4_addr("0.0.0.0", cfg->port, (struct sockaddr_in *) &addr);
    }
    else
    {
        uv_ip6_addr("::", cfg->port, (struct sockaddr_in6 *) &addr);
    }

    if ((rc = uv_tcp_bind(
            &back->sock->tcp,
            (const struct sockaddr *) &addr,
            (cfg->ip_support == AF_INET6) ?
                    UV_TCP_IPV6ONLY : 0)) ||

        (rc = uv_listen(
            (uv_stream_t *) &back->sock->tcp,
            THINGSDB_MAX_NODES,
            (uv_connection_cb) ti__back_on_connect)))
    {
        log_error("error listening for nodes: %s", uv_strerror(rc));
        return -1;
    }

    log_info("start listening for nodes on port %d", cfg->port);
    return 0;
}

const char * ti_back_req_str(ti_back_req_e tp)
{
    switch (tp)
    {
    case TI_BACK_PING: return "REQ_PING";
    case TI_BACK_AUTH: return "REQ_AUTH";
    case TI_BACK_EVENT_REG: return "REQ_EVENT_REG";
    case TI_BACK_EVENT_UPD: return "REQ_EVENT_UPD";
    case TI_BACK_EVENT_READY: return "REQ_EVENT_READY";
    case TI_BACK_EVENT_CANCEL: return "REQ_EVENT_CANCEL";
    case TI_BACK_MAINT_REG: return "REQ_MAINT_REG";
    }
    return "REQ_UNKNOWN";
}

static void ti__back_on_connect(uv_tcp_t * tcp, int status)
{
    int rc;

    if (status < 0)
    {
        log_error("node connection error: %s", uv_strerror(status));
        return;
    }

    ti_sock_t * nsock = ti_sock_create(TI_SOCK_NODE);
    if (!nsock || ti_sock_init(nsock)) return;
    nsock->cb = ti__back_on_pkg;
    if ((rc = uv_accept((uv_stream_t *) tcp, (uv_stream_t *) &nsock->tcp)) ||
        (rc = uv_read_start(
                (uv_stream_t *) &nsock->tcp,
                ti_sock_alloc_buf,
                ti_sock_on_data)))
    {
        log_error(uv_strerror(rc));
        ti_sock_close(nsock);
        return;
    }
    log_info("node connected: %s", ti_sock_addr(nsock));
}

static void ti__back_on_pkg(ti_sock_t * sock, ti_pkg_t * pkg)
{
    switch (pkg->tp)
    {
    default:
        log_error("test sock flags: %u", sock->flags);
    }
}