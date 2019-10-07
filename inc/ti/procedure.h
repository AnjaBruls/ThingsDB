/*
 * ti/procedure.h
 */
#ifndef TI_PROCEDURE_H_
#define TI_PROCEDURE_H_

typedef struct ti_procedure_s ti_procedure_t;

#include <cleri/cleri.h>
#include <inttypes.h>
#include <util/vec.h>
#include <ti/syntax.h>
#include <ex.h>
#include <ti/query.h>
#include <ti/raw.h>
#include <ti/closure.h>
#include <ti/val.h>

ti_procedure_t * ti_procedure_create(ti_raw_t * name, ti_closure_t * closure);
void ti_procedure_destroy(ti_procedure_t * procedure);

ti_procedure_t * ti_procedure_from_raw(
        ti_raw_t * def,
        ti_syntax_t * syntax,
        ex_t * e);
ti_procedure_t * ti_procedure_from_strn(
        const char * str,
        size_t n,
        ti_syntax_t * syntax,
        ex_t * e);
int ti_procedure_info_to_pk(
        ti_procedure_t * procedure,
        qp_packer_t ** packer);
ti_val_t * ti_procedure_info_as_qpval(ti_procedure_t * procedure);

struct ti_procedure_s
{
    ti_raw_t * name;            /* name of the procedure */
    ti_raw_t * doc;             /* documentation, may be NULL */
    ti_raw_t * def;             /* definition, may be NULL */
    ti_closure_t * closure;     /* closure */
};


#endif /* TI_PROCEDURE_H_ */
