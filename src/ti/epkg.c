/*
 * ti/epkg.c
 */
#include <assert.h>
#include <ti/epkg.h>
#include <ti/auth.h>
#include <ti/proto.h>
#include <ti.h>
#include <stdlib.h>
#include <util/qpx.h>
#include <util/cryptx.h>

ti_epkg_t * ti_epkg_create(ti_pkg_t * pkg, uint64_t event_id)
{
    ti_epkg_t * epkg = malloc(sizeof(ti_epkg_t));
    if (!epkg)
        return NULL;
    epkg->ref = 1;
    epkg->pkg = pkg;
    epkg->event_id = event_id;
    return epkg;
}

ti_epkg_t * ti_epkg_initial(void)
{
    uint64_t event_id = 1;
    uint64_t target_id = 0;
    uint64_t thing_id = 0;
    ti_epkg_t * epkg;
    ti_pkg_t * pkg;
    qpx_packer_t * packer;
    char salt[CRYPTX_SALT_SZ];
    char encrypted[CRYPTX_SZ];

    /* generate a random salt */
    cryptx_gen_salt(salt);

    /* encrypt the users password */
    cryptx(ti_user_def_pass, salt, encrypted);

    packer = qpx_packer_create(128, 5);
    if (!packer)
        return NULL;

    (void) qp_add_map(&packer);

    (void) qp_add_array(&packer);
    (void) qp_add_int64(packer, event_id);
    (void) qp_add_int64(packer, target_id);
    (void) qp_close_array(packer);

    (void) qp_add_map(&packer);

    (void) qp_add_int64(packer, thing_id);
    (void) qp_add_array(&packer);

    (void) qp_add_map(&packer);
    (void) qp_add_raw_from_str(packer, "new_user");
    (void) qp_add_map(&packer);
    (void) qp_add_raw_from_str(packer, "id");
    (void) qp_add_int64(packer, ti_next_thing_id());
    (void) qp_add_raw_from_str(packer, "username");
    (void) qp_add_raw_from_str(packer, ti_user_def_name);
    (void) qp_add_raw_from_str(packer, "password");
    (void) qp_add_raw_from_str(packer, encrypted);
    (void) qp_close_map(packer);
    (void) qp_close_map(packer);

    (void) qp_add_map(&packer);
    (void) qp_add_raw_from_str(packer, "grant");
    (void) qp_add_map(&packer);
    (void) qp_add_raw_from_str(packer, "target");
    (void) qp_add_int64(packer, 0);
    (void) qp_add_raw_from_str(packer, "user");
    (void) qp_add_raw_from_str(packer, ti_user_def_name);
    (void) qp_add_raw_from_str(packer, "mask");
    (void) qp_add_int64(packer, TI_AUTH_MASK_FULL);
    (void) qp_close_map(packer);
    (void) qp_close_map(packer);

    (void) qp_close_array(packer);

    (void) qp_close_map(packer);
    (void) qp_close_map(packer);
    (void) qp_close_map(packer);

    pkg = qpx_packer_pkg(packer, TI_PROTO_NODE_EVENT);
    epkg = ti_epkg_create(pkg, event_id);
    if (!epkg)
    {
        free(pkg);
        return NULL;
    }
    return epkg;
}
