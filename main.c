#include <locale.h>
#include <stdlib.h>
#include <ti/store.h>
#include <ti/user.h>
#include <ti/version.h>
#include <ti/store.h>
#include <ti.h>
#include <util/fx.h>


int main(int argc, char * argv[])
{
    ti_t * ti;
    int rc = EXIT_SUCCESS;

    /* set local to LC_ALL */
    (void) setlocale(LC_ALL, "");

    /* initialize random */
    srand(time(NULL));

    /* set thread-pool size to 4 (default=4) */
    putenv("UV_THREADPOOL_SIZE=4");

    /* set default time-zone to UTC */
    putenv("TZ=:UTC");
    tzset();

    rc = ti_create();
    if (!rc)
        goto stop;

    ti = ti_get();

    /* parse arguments */
    if ((rc = ti_args_parse(argc, argv)))
        goto stop;

    if (ti->args->version)
    {
        ti_version_print();
        goto stop;
    }

    ti_init_logger();

    rc = ti_cfg_parse(ti->args->config);
    if (rc)
        goto stop;

    rc = ti_lock();
    if (rc)
        goto stop;

    rc = ti_init_fn();
    if (rc)
        goto stop;

    if (ti->args->init)
    {
        if (fx_file_exist(ti->fn))
        {
            printf("error: directory `%s` is already initialized\n",
                    ti->cfg->store_path);
            rc = -1;
            goto stop;
        }
        if ((rc = ti_build()))
        {
            printf("error: building new pool has failed\n");
            goto stop;
        }

        printf(
            "Well done! You successfully initialized a new ThingsDB pool.\n\n"
            "You can now start ThingsDB and connect by using the default user `%s`.\n"
            "..before I forget, the password is `%s`\n\n",
            ti_user_def_name,
            ti_user_def_pass);

        goto stop;
    }

    if (strlen(ti->args->secret))
    {
        printf(
            "Waiting for a invite to join some pool from a ThingsDB node...\n"
            "(if you want to create a new pool instead, press CTRL+C and "
            "use the --init argument)\n");
    }
    else if (fx_file_exist(ti->fn))
    {
        if ((rc = ti_read()))
        {
            printf("error reading tin pool from: '%s'\n", ti->fn);
            goto stop;
        }

        if ((rc = ti_store_restore()))
        {
            printf("error loading tin pool\n");
            goto stop;
        }
    }
    else
    {
        printf(
            "The first time you should either create a new pool using "
            "the --init argument or set a one-time-secret using the --secret "
            "argument and wait for a invite from another node.\n");
        goto stop;
    }

    rc = ti_run();
stop:
    if (ti_unlock() || rc)
    {
        rc = EXIT_FAILURE;
    }
    ti_destroy();

    return rc;
}
