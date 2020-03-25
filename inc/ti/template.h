/*
 * ti/template.h
 */
#ifndef TI_TEMPLATE_H_
#define TI_TEMPLATE_H_

typedef struct ti_template_s ti_template_t;

#include <ti/query.h>
#include <ex.h>
#include <cleri/cleri.h>

int ti_template_build(cleri_node_t * node);
int ti_template_compile(ti_template_t * template, ti_query_t * query, ex_t * e);
void ti_template_destroy(ti_template_t * template);

struct ti_template_s
{
    uint32_t ref;
    uint8_t tp;
    uint8_t _flags;
    uint16_t _pad16;
    cleri_node_t * node;
};


#endif  /* TI_TEMPLATE_H_ */