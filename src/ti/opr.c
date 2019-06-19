/*
 * ti/opr.c
 */
#include <ti/opr/add.h>
#include <ti/opr/and.h>
#include <ti/opr/div.h>
#include <ti/opr/eq.h>
#include <ti/opr/ge.h>
#include <ti/opr/gt.h>
#include <ti/opr/idiv.h>
#include <ti/opr/le.h>
#include <ti/opr/lt.h>
#include <ti/opr/mod.h>
#include <ti/opr/mul.h>
#include <ti/opr/ne.h>
#include <ti/opr/or.h>
#include <ti/opr/sub.h>
#include <ti/opr/xor.h>


int ti_opr_a_to_b(ti_val_t * a, cleri_node_t * nd, ti_val_t ** b, ex_t * e)
{
    switch (nd->len)
    {
    case 1:
        switch (*nd->str)
        {
        case '%':
            return opr__mod(a, b, e);
        case '&':
            return opr__and(a, b, e);
        case '*':
            return opr__mul(a, b, e);
        case '+':
            return opr__add(a, b, e);
        case '-':
            return opr__sub(a, b, e);
        case '/':
            return opr__div(a, b, e);
        case '<':
            return opr__lt(a, b, e);
        case '>':
            return opr__gt(a, b, e);
        case '^':
            return opr__xor(a, b, e);
        case '|':
            return opr__or(a, b, e);
        }
        break;
    case 2:
        switch (*nd->str)
        {
        case '!':
            assert (nd->str[1] == '=');
            return opr__ne(a, b, e);
        case '%':
            assert (nd->str[1] == '=');
            return opr__mod(a, b, e);
        case '&':
            assert (nd->str[1] == '=');
            return opr__and(a, b, e);
        case '*':
            assert (nd->str[1] == '=');
            return opr__mul(a, b, e);
        case '+':
            assert (nd->str[1] == '=');
            return opr__add(a, b, e);
        case '-':
            assert (nd->str[1] == '=');
            return opr__sub(a, b, e);
        case '/':
            return nd->str[1] == '=' && a->tp == TI_VAL_FLOAT
                ? opr__div(a, b, e)
                : opr__idiv(a, b, e);
        case '<':
            assert (nd->str[1] == '=');
            return opr__le(a, b, e);
        case '=':
            assert (nd->str[1] == '=');
            return opr__eq(a, b, e);
        case '>':
            assert (nd->str[1] == '=');
            return opr__ge(a, b, e);
        case '^':
            assert (nd->str[1] == '=');
            return opr__xor(a, b, e);
        case '|':
            assert (nd->str[1] == '=');
            return opr__or(a, b, e);
        }
    }
    assert (0);
    return e->nr;
}

_Bool ti_opr_eq(ti_val_t * a, ti_val_t * b)
{
    switch ((ti_val_enum) a->tp)
    {
    case TI_VAL_NIL:
        return  a->tp == b->tp;
    case TI_VAL_INT:
        switch ((ti_val_enum) b->tp)
        {
        case TI_VAL_NIL:
            return false;
        case TI_VAL_INT:
            return OPR__INT(a) == OPR__INT(b);
        case TI_VAL_FLOAT:
            return OPR__INT(a) == OPR__FLOAT(b);
        case TI_VAL_BOOL:
            return OPR__INT(a) == OPR__BOOL(b);
        case TI_VAL_QP:
        case TI_VAL_RAW:
        case TI_VAL_REGEX:
        case TI_VAL_THING:
        case TI_VAL_ARR:
        case TI_VAL_SET:
        case TI_VAL_CLOSURE:
            return false;
        }
        break;
    case TI_VAL_FLOAT:
        switch ((ti_val_enum) b->tp)
        {
        case TI_VAL_NIL:
            return false;
        case TI_VAL_INT:
            return OPR__FLOAT(a) == OPR__INT(b);
        case TI_VAL_FLOAT:
            return OPR__FLOAT(a) == OPR__FLOAT(b);
        case TI_VAL_BOOL:
            return OPR__FLOAT(a) == OPR__BOOL(b);
        case TI_VAL_QP:
        case TI_VAL_RAW:
        case TI_VAL_REGEX:
        case TI_VAL_THING:
        case TI_VAL_ARR:
        case TI_VAL_SET:
        case TI_VAL_CLOSURE:
            return false;
        }
        break;
    case TI_VAL_BOOL:
        switch ((ti_val_enum) b->tp)
        {
        case TI_VAL_NIL:
            return false;
        case TI_VAL_INT:
            return  OPR__BOOL(a) == OPR__INT(b);
        case TI_VAL_FLOAT:
            return OPR__BOOL(a) == OPR__FLOAT(b);
        case TI_VAL_BOOL:
            return OPR__BOOL(a) == OPR__BOOL(b);
        case TI_VAL_QP:
        case TI_VAL_RAW:
        case TI_VAL_REGEX:
        case TI_VAL_THING:
        case TI_VAL_ARR:
        case TI_VAL_SET:
        case TI_VAL_CLOSURE:
            return false;
        }
        break;
    case TI_VAL_QP:
    case TI_VAL_RAW:
        switch ((ti_val_enum) b->tp)
        {
        case TI_VAL_NIL:
        case TI_VAL_INT:
        case TI_VAL_FLOAT:
        case TI_VAL_BOOL:
            return false;
        case TI_VAL_QP:
        case TI_VAL_RAW:
            return ti_raw_equal((ti_raw_t *) a, (ti_raw_t *) b);
        case TI_VAL_REGEX:
        case TI_VAL_THING:
        case TI_VAL_ARR:
        case TI_VAL_SET:
        case TI_VAL_CLOSURE:
            return false;
        }
        break;
    case TI_VAL_REGEX:
    case TI_VAL_THING:
    case TI_VAL_ARR:
    case TI_VAL_SET:
    case TI_VAL_CLOSURE:
        return a == b;
    }
    return false;
}
