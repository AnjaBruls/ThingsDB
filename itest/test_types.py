#!/usr/bin/env python
import asyncio
import pickle
import time
from lib import run_test
from lib import default_test_setup
from lib.testbase import TestBase
from lib.client import get_client
from thingsdb.exceptions import AssertionError
from thingsdb.exceptions import ValueError
from thingsdb.exceptions import TypeError
from thingsdb.exceptions import NumArgumentsError
from thingsdb.exceptions import BadDataError
from thingsdb.exceptions import LookupError
from thingsdb.exceptions import OverflowError
from thingsdb.exceptions import ZeroDivisionError
from thingsdb.exceptions import OperationError


class TestTypes(TestBase):

    title = 'Test thingsdb types'

    @default_test_setup(num_nodes=1, seed=1)
    async def run(self):

        await self.node0.init_and_run()

        client = await get_client(self.node0)
        client.set_default_scope('//stuff')

        await self.run_tests(client)

        client.close()
        await client.wait_closed()

    async def test_regex(self, client):
        with self.assertRaisesRegex(
                ValueError,
                r'cannot compile regular expression \'/invalid\(regex/\', '
                r'missing closing parenthesis'):
            await client.query(r'r = /invalid(regex/;')

    async def test_comment(self, client):
        self.assertIs(await client.query(r'/* comment */'), None)
        self.assertIs(await client.query(r'// comment'), None)

    async def test_raw(self, client):
        self.assertEqual(await client.query(r'''
            "Hi ""Iris""!!";
        '''), 'Hi "Iris"!!')
        self.assertEqual(await client.query(r'''
            'Hi ''Iris''!!';
        '''), "Hi 'Iris'!!")
        self.assertTrue(await client.query(r'''
            ("Hi ""Iris""" == 'Hi "Iris"' && 'Hi ''Iris''!' == "Hi 'Iris'!")
        '''))
        self.assertEqual(await client.query(r'''
            blob;
        ''', blob="Hi 'Iris'!!"), "Hi 'Iris'!!")
        self.assertTrue(await client.query(' ("Hello"[0] == "H") '))
        self.assertTrue(await client.query(' ("Hello"[0][-1] == "H") '))
        self.assertTrue(await client.query(' ("Hello"[-1] == "o") '))
        self.assertTrue(await client.query(' ("Hello"[-4] == "e") '))
        self.assertTrue(await client.query(' ("Hello" == "Hello") '))
        self.assertTrue(await client.query(' ("Hello" != "hello") '))
        self.assertTrue(await client.query(' ("Hello" != "Hello.") '))

    async def test_thing(self, client):
        self.assertEqual(await client.query(r'''
            [{}.id(), [{}][0].id()];
        '''), [None, None])

        self.assertEqual(await client.query(r'''
            tmp = {a: [{}], b: {}};
            [tmp.a[0].id(), tmp.b.id(), tmp.id()];
        '''), [None, None, None])

        self.assertEqual(await client.query(r'''
           {t: 0}.t;
        '''), 0)

        self.assertEqual(await client.query(r'''
           return({a: {t: 0}}, 0);
        '''), {})

        self.assertEqual(await client.query(r'''
           return({a: {t: 0}}, 1);
        '''), {'a': {}})

        self.assertEqual(await client.query(r'''
           return({a: {t: 0}}, 2);
        '''), {'a': {'t': 0}})

        self.assertGreater(await client.query(r'''
            tmp = {t: {}};
            .t = tmp.t;
            tmp.t.id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .t = {};
            .t.id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .a = [{}];
            .a[0].id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .a = [[{}]];
            .a[0][0].id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .t = {t: {}};
            .t.t.id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .t = {l: []};
            .t.l.push({a: {}});
            .t.l[0].id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            .t.l[0].a.id();
        '''), 0)

        self.assertEqual(await client.query(r'''
            t = {l: []};
            t.l.push({a: {}});
            [t.l[0].id(), t.l[0].a.id()];
        '''), [None, None])

    async def test_list(self, client):
        self.assertEqual(await client.query(r'''
            ({}.t = [1, 2, 3]).push(4);
        '''), 4)

    async def test_closure(self, client):
        with self.assertRaisesRegex(
                OverflowError,
                'integer overflow'):
            await client.query('.x = ||(1+999999999999999999999);')

        with self.assertRaisesRegex(
                OperationError,
                r'maximum recursion depth exceeded'):
            await client.query(r'''
                .a = ||.map((b = .a));
                .map(.a);
            ''')

        with self.assertRaisesRegex(
                OperationError,
                r'stored closures with side effects must be '
                r'wrapped using '):
            await client.query(r'''
                .b = ||(.x = 1);
                [1 ,2 ,3].map(.b);
            ''')

        with self.assertRaisesRegex(
                OperationError,
                r'stored closures with side effects must be '
                r'wrapped using '):
            await client.query(r'''
                .a = [||.x = 1];
                [1 ,2 ,3].map(.a[0]);
            ''')

        # test two-level deep nesting
        self.assertEqual(await client.query(r'''
            .b = |k1|.map(|k2|(k1 + k2));
            .map(.b);
        '''), [["aa", "ab"], ["ba", "bb"]])

        self.assertEqual(await client.query(r'''
            res = [];
            closure = || {
                a = 1;
                .c = closure;  /* store the closure, this will unbound the
                                * closure from the query */
                a += 1;
                res.push(a);
            };
            closure();
            res;
        '''), [2])

        self.assertEqual(await client.query(r'''
            res = [];
            c = |x, y, i, c| {
                a = x + i;
                b = y + i;
                i = i + 1;  // writes a variable `i` in the local scope
                if (i < 4, {
                    c(x, y, i, c);
                });
                c = a + b;
                res.push([i-1, c]);
            };
            c(7, 5, 0, c);
            res;
        '''), [[3, 18], [2, 16], [1, 14], [0, 12]])

    async def test_integer(self, client):
        with self.assertRaisesRegex(
                OverflowError,
                'integer overflow'):
            await client.query('9999999999999999999;')

    async def test_set(self, client):
        self.assertTrue(await client.query(r'''
            ( set() == set() )
        '''))

        self.assertTrue(await client.query(r'''
            .a = {}; .b = .a;
            ( set(.a, .a) == set(.b,) )
        '''))

        self.assertTrue(await client.query(r'''
            .a = {};
            ( set([.a]) != set([]) )
        '''))

        self.assertTrue(await client.query(r'''
            .a = {}; .b = {};
            ( set([.a]) != set([.b]) )
        '''))
        await client.query(r'''
            anna = {};
            cato = {};
            iris = {};

            a = set(cato, iris);
            b = set(cato, anna);

            assert (a | b == set(anna, cato, iris));    // Union
            assert (a & b == set(cato));                // Intersection
            assert (a - b == set(iris));                // Difference
            assert (a ^ b == set(anna, iris));          // Symmetric difference
        ''')


if __name__ == '__main__':
    run_test(TestTypes())
