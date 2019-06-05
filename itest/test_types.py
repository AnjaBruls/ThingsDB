#!/usr/bin/env python
import asyncio
import pickle
import time
from lib import run_test
from lib import default_test_setup
from lib.testbase import TestBase
from lib.client import get_client
from thingsdb.exceptions import AssertionError
from thingsdb.exceptions import BadRequestError
from thingsdb.exceptions import IndexError
from thingsdb.exceptions import OverflowError


class TestTypes(TestBase):

    title = 'Test ThingsDB types'

    @default_test_setup(num_nodes=1, seed=1)
    async def run(self):

        await self.node0.init_and_run()

        client = await get_client(self.node0)
        client.use('stuff')

        await self.run_tests(client)

        client.close()
        await client.wait_closed()

    async def test_regex(self, client):
        with self.assertRaisesRegex(
                BadRequestError,
                r'cannot compile regular expression \'/invalid\(regex/\', '
                r'missing closing parenthesis'):
            await client.query(r'r = /invalid(regex/;')

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
            blob(0);
        ''', blobs=["Hi 'Iris'!!"]), "Hi 'Iris'!!")

    async def test_thing(self, client):
        self.assertEqual(await client.query(r'''
            [{}.id(), [{}][0].id()];
        '''), [0, 0])

        self.assertEqual(await client.query(r'''
            $tmp = {a: [{}], b: {}};
            [$tmp.a[0].id(), $tmp.b.id(), $tmp.id()];
        '''), [0, 0, 0])

        self.assertEqual(await client.query(r'''
           {t: 0}.t;
        '''), 0)

        self.assertGreater(await client.query(r'''
            $tmp = {t: {}};
            t = $tmp.t;
            $tmp.t.id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            t = {};
            t.id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            a = [{}];
            a[0].id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            a = [[{}]];
            a[0][0].id();
        '''), 0)

        self.assertGreater(await client.query(r'''
            t = {t: {}};
            t.t.id();
        '''), 0)


if __name__ == '__main__':
    run_test(TestTypes())