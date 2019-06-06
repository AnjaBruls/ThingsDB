/*
 * ti/pkg.c
 */
#include <assert.h>
#include <string.h>
#include <qpack.h>
#include <stdlib.h>
#include <ti/pkg.h>
#include <ti/proto.h>
#include <util/qpx.h>
#include <util/logger.h>

ti_pkg_t * ti_pkg_new(
        uint16_t id,
        uint8_t tp,
        const unsigned char * data,
        uint32_t n)
{
    ti_pkg_t * pkg = malloc(sizeof(ti_pkg_t) + n);

    if (!pkg)
        return NULL;

    pkg->tp = tp;
    pkg->ntp = tp ^ 0xff;
    pkg->n = n;
    pkg->id = id;

    memcpy(pkg->data, data, n);

    return pkg;
}

ti_pkg_t * ti_pkg_dup(ti_pkg_t * pkg)
{
    size_t sz = sizeof(ti_pkg_t) + pkg->n;
    ti_pkg_t * dup = malloc(sz);
    if (!dup)
        return NULL;
    memcpy(dup, pkg, sz);
    return dup;
}

ti_pkg_t * ti_pkg_client_err(uint16_t id, ex_t * e)
{
    uint8_t tp;
    ti_pkg_t * pkg;
    qpx_packer_t * xpkg = qpx_packer_create(30 + e->n, 1);
    if (!xpkg)
        return NULL;
    switch (e->nr)
    {
    case EX_OVERFLOW:
        tp = TI_PROTO_CLIENT_ERR_OVERFLOW;
        break;
    case EX_ZERO_DIV:
        tp = TI_PROTO_CLIENT_ERR_ZERO_DIV;
        break;
    case EX_MAX_QUOTA:
        tp = TI_PROTO_CLIENT_ERR_MAX_QUOTA;
        break;
    case EX_AUTH_ERROR:
        tp = TI_PROTO_CLIENT_ERR_AUTH;
        break;
    case EX_FORBIDDEN:
        tp = TI_PROTO_CLIENT_ERR_FORBIDDEN;
        break;
    case EX_INDEX_ERROR:
        tp = TI_PROTO_CLIENT_ERR_INDEX;
        break;
    case EX_BAD_DATA:
        tp = TI_PROTO_CLIENT_ERR_BAD_REQUEST;
        break;
    case EX_SYNTAX_ERROR:
        tp = TI_PROTO_CLIENT_ERR_SYNTAX;
        break;
    case EX_NODE_ERROR:
        tp = TI_PROTO_CLIENT_ERR_NODE;
        break;
    case EX_ASSERT_ERROR:
        tp = TI_PROTO_CLIENT_ERR_ASSERTION;
        break;
    case EX_REQUEST_TIMEOUT:
    case EX_REQUEST_CANCEL:
    case EX_WRITE_UV:
    case EX_ALLOC:
    case EX_INTERNAL:
        tp = TI_PROTO_CLIENT_ERR_INTERNAL;
        break;
    case EX_SUCCESS:
    default:
        assert (e->nr >= 0);
        tp = TI_PROTO_CLIENT_ERR_ASSERTION;
    }

    (void) qp_add_map(&xpkg);
    (void) qp_add_raw(xpkg, (const unsigned char *) "error_code", 10);
    /* return the `custom` error code when e->nr > 0, if e->nr < 0, return
     * the package type which is equal to the known error codes in the
     * documentation.
     */
    (void) qp_add_int(xpkg, e->nr < 0 ? -((int) tp) : e->nr);
    (void) qp_add_raw(xpkg, (const unsigned char *) "error_msg", 9);
    (void) qp_add_raw(xpkg, (const unsigned char *) e->msg, e->n);
    (void) qp_close_map(xpkg);

    pkg = qpx_packer_pkg(xpkg, tp);
    pkg->id = id;

    return pkg;
}

ti_pkg_t * ti_pkg_node_err(uint16_t id, ex_t * e)
{
    return ti_pkg_new(id, TI_PROTO_NODE_ERR_RES, (const uchar *) e->msg, e->n);
}

void ti_pkg_log(ti_pkg_t * pkg)
{
    switch (pkg->tp)
    {
    case TI_PROTO_NODE_ERR_RES:
        log_error("error response on `package:%u`: %.*s",
                pkg->id, (int) pkg->n, (char *) pkg->data);
        break;
    default:
        log_info("package id: %u, type: %s", pkg->id, ti_proto_str(pkg->tp));
    }
}

void ti_pkg_set_tp(ti_pkg_t * pkg, uint8_t tp)
{
    pkg->tp = tp;
    pkg->ntp = tp ^ 0xff;
}
