/*
 * ti/queryi.h
 */
#ifndef TI_QUERYI_H_
#define TI_QUERYI_H_

#include <util/vec.h>
#include <ti.h>
#include <ti/query.h>
#include <ti/syntax.h>

static inline vec_t * ti_query_access(ti_query_t * query)
{
    return query->syntax.flags & TI_SYNTAX_FLAG_COLLECTION
            ? query->collection->access
            : query->syntax.flags & TI_SYNTAX_FLAG_THINGSDB
            ? ti()->access_thingsdb
            : query->syntax.flags & TI_SYNTAX_FLAG_NODE
            ? ti()->access_node
            : NULL;
}

#endif  /* TI_QUERYI_H_ */
