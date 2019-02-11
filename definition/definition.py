import re
import sys
from pyleri import (
    Grammar,
    Keyword,
    Regex,
    Choice as Choice_,
    Sequence,
    Ref,
    Prio,
    Token,
    Tokens,
    Repeat,
    List as List_,
    Optional,
    THIS,
)

RE_NAME = r'^[A-Za-z_][0-9A-Za-z_]*'
RE_TMP = r'^\$[A-Za-z_][0-9A-Za-z_]*'
ASSIGN_TOKENS = '= += -= *= /= %= &= ^= |='


class Choice(Choice_):
    def __init__(self, *args, most_greedy=None, **kwargs):
        if most_greedy is None:
            most_greedy = False
        super().__init__(*args, most_greedy=most_greedy, **kwargs)


class List(List_):
    def __init__(self, *args, opt=None, **kwargs):
        if opt is None:
            opt = True
        super().__init__(*args, opt=opt, **kwargs)


class Definition(Grammar):
    RE_KEYWORDS = re.compile(RE_NAME)

    r_single_quote = Regex(r"(?:'(?:[^']*)')+")
    r_double_quote = Regex(r'(?:"(?:[^"]*)")+')

    t_false = Keyword('false')
    t_float = Regex(r'[-+]?((inf|nan)([^0-9A-Za-z_]|$)|[0-9]*\.[0-9]+(e[+-][0-9]+)?)')
    t_int = Regex(r'[-+]?((0b[01]+)|(0o[0-8]+)|(0x[0-9a-fA-F]+)|([0-9]+))')
    t_nil = Keyword('nil')
    t_regex = Regex('(/[^/\\\\]*(?:\\\\.[^/\\\\]*)*/i?)')
    t_string = Choice(r_single_quote, r_double_quote)
    t_true = Keyword('true')

    o_not = Repeat(Token('!'))
    comment = Repeat(Regex(r'(?s)/\\*.*?\\*/'))

    name = Regex(RE_NAME)
    tmp = Regex(RE_TMP)

    # build-in get functions
    f_blob = Keyword('blob')
    f_endswith = Keyword('endswith')
    f_filter = Keyword('filter')
    f_find = Keyword('find')
    f_get = Keyword('get')
    f_hasprop = Keyword('hasprop')
    f_id = Keyword('id')
    f_int = Keyword('int')
    f_isarray = Keyword('isarray')
    f_isinf = Keyword('isinf')
    f_islist = Keyword('islist')
    f_isnan = Keyword('isnan')
    f_len = Keyword('len')
    f_lower = Keyword('lower')
    f_map = Keyword('map')
    f_now = Keyword('now')
    f_ret = Keyword('ret')
    f_startswith = Keyword('startswith')
    f_str = Keyword('str')
    f_test = Keyword('test')
    f_thing = Keyword('thing')
    f_try = Keyword('try')
    f_upper = Keyword('upper')

    # build-in update functions
    f_del = Keyword('del')
    f_push = Keyword('push')
    f_rename = Keyword('rename')
    f_set = Keyword('set')
    f_splice = Keyword('splice')
    f_unset = Keyword('unset')

    primitives = Choice(
        t_false,
        t_nil,
        t_true,
        t_float,
        t_int,
        t_string,
        t_regex,
    )

    scope = Ref()
    chain = Ref()

    thing = Sequence('{', List(Sequence(name, ':', scope)), '}')
    array = Sequence('[', List(scope), ']')

    arrow = Sequence(List(tmp, opt=False), '=>', scope)

    function = Sequence(Choice(
        # build-in get functions
        f_blob,         # (int inx_in_blobs) -> raw
        f_endswith,     # (str) -> bool
        f_filter,       # (arrow) -> [return values where return is true]
        f_find,         # (arrow) -> return first value where true or null
        f_get,          # (str,..) -> attribute val
        f_hasprop,      # (str) -> bool
        f_id,           # () -> int
        f_int,          # (x) -> int
        f_isarray,      # (x) -> bool
        # f_isascci,
        # f_isbool,
        # f_isfloat,
        # f_isint,
        # f_isnumber,
        # f_israw,
        # f_isstr,        # alias for isutf8 (if isutf8, then is isascii)
        # f_isthings,
        # f_isutf8,
        f_isinf,        # (float) -> bool
        f_islist,       # (x) -> bool
        f_isnan,        # (float) -> bool
        f_len,          # () -> int
        f_lower,        # () -> str
        f_map,          # (arrow) -> [return values]
        f_now,          # () -> timestamp as double seconds.nanoseconds
        f_ret,          # () -> nil
        f_startswith,   # (str) -> bool
        f_str,          # (x) -> raw
        f_test,         # (regex) -> bool
        f_thing,        # (int thing_id) -> thing
        f_try,
        f_upper,        # () -> str
        # build-in update functions
        f_del,
        f_push,
        f_rename,       # TODO : implement
        f_set,
        f_splice,       # TODO : implement
        f_unset,
        # any name
        name,           # used for `root` functions
    ), '(', List(scope), ')')

    opr0_mul_div_mod = Tokens('* / % //')
    opr1_add_sub = Tokens('+ -')
    opr2_bitwise_and = Tokens('&')
    opr3_bitwise_xor = Tokens('^')
    opr4_bitwise_or = Tokens('|')
    opr5_compare = Tokens('< > == != <= >=')
    opr6_cmp_and = Token('&&')
    opr7_cmp_or = Token('||')

    operations = Sequence(
        '(',
        Prio(
            scope,
            Sequence(THIS, Choice(
                opr0_mul_div_mod,
                opr1_add_sub,
                opr2_bitwise_and,
                opr3_bitwise_xor,
                opr4_bitwise_or,
                opr5_compare,
                opr6_cmp_and,
                opr7_cmp_or,
                most_greedy=True,
            ), THIS)
        ),
        ')',
        Optional(Sequence('?', scope, ':', scope)),  # conditional support?
    )

    assignment = Sequence(name, Tokens(ASSIGN_TOKENS), scope)
    tmp_assign = Sequence(tmp, Tokens(ASSIGN_TOKENS), scope)

    index = Repeat(
        Sequence('[', t_int, ']')
    )       # we skip index in query investigate (in case we want to use scope)

    chain = Sequence(
        '.',
        Choice(function, assignment, name),
        index,
        Optional(chain),
    )

    scope = Sequence(
        o_not,
        Choice(
            primitives,
            function,
            assignment,
            tmp_assign,
            arrow,
            name,
            tmp,
            thing,
            array,
            operations,
        ),
        index,
        Optional(chain),
    )

    START = Sequence(
        comment,
        List(scope, delimiter=Sequence(';', comment)),
    )

    @classmethod
    def translate(cls, elem):
        if elem == cls.name:
            return 'name'

    def test(self, str):
        print('{} : {}'.format(
            str.strip(), self.parse(str).as_str(self.translate)))


if __name__ == '__main__':
    definition = Definition()

    definition.test(r'''

        (true) ? 2 : 3;

        Oversight = {
            redundancy: 3,
            nodes: [],
        };

        inf''')
    exit(0)

    definition.test('users.find(user => (user.id == 1)).labels.filter(label => (label.id().i == 1))')
    definition.test('users.find(user => (user.id == 1)).labels.filter(label => (label.id().i == 1))')
    # exit(0)
    definition.test('users.create("iris");grant(users.iris,FULL)')
    definition.test('labels.map(label => label.id())')
    definition.test('''
        /*
         * Create a collection
         */
        collections.create(dbtest);

        /*
         * Drop a collection
         */
        collections.dbtest.drop();

        /* Change redundancy */
        config.redundancy = 3[0][0];

        types().create(User, {
            name: str().required(),
            age: int().required(),
            owner: User.required(),
            schools: School.required(),
            scores: thing,
            other: int
        });
        users.new({
            name: 'iris'
        });

        type().add(users, User.isRequired);

        bla.set('x', 4);

        /*
         * Finished!
         */
    ''')

    definition.test(' users.new({name: "iris"}); ')
    definition.test(' collections.dbtest.drop() ')

    definition.test('2.1')

    {
        '$ev': 0,
        '#': 4,
        '$jobs': [
            {'assign': {'age', 5}},
            {'del': 'age'},
            {'set': {'name': 'iris'}},
            {'set': {'image': '<bin_data>'}},
            {'unset': 'name'},
            {'push': {'people': [{'#': 123}]}}
        ]
    }

    {
        'event': 1,
        '#': 0,
        'jobs': [
            {'new': {'Database': {
                'name': 'testdb',
                'user': 'iris'
            }}}
        ]
    }

    c, h = definition.export_c(target='langdef', headerf='<langdef/langdef.h>')
    with open('../src/langdef/langdef.c', 'w') as cfile:
        cfile.write(c)

    with open('../inc/langdef/langdef.h', 'w') as hfile:
        hfile.write(h)

    print('Finished export to c')
