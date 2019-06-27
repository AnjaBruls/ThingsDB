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
from thingsdb.exceptions import ZeroDivisionError
from thingsdb import scope


class TestNested(TestBase):

    title = 'Test nested and more complex queries'

    @default_test_setup(num_nodes=2, seed=1)
    async def run(self):

        await self.node0.init_and_run()

        client = await get_client(self.node0)
        client.use('stuff')

        await self.run_tests(client)
        # return  # uncomment to skip garbage collection test

        # add another node so away node and gc is forced
        await self.node1.join_until_ready(client)

        # expected no garbage collection
        counters = await client.query('counters();', target=scope.node)
        self.assertEqual(counters['garbage_collected'], 0)

        client.close()
        await client.wait_closed()

    async def test_nested_closure_query(self, client):
        usera, userb = await client.query(r'''
            channel = {};
            workspace = {channels: [channel]};
            usera = {
                memberships: [{
                    workspace: workspace,
                    channels: [channel]
                }]
            };
            userb = {
                memberships: [{
                    workspace: workspace,
                    channels: []
                }]
            };
            users = [usera, userb];
        ''')

        self.assertEqual(await client.query(r'''
            users.filter(
                |user| !isnil(
                    user.memberships.find(
                        |membership| (
                            membership.workspace == workspace
                        )
                    ).channels.indexof(channel)
                )
            );
        '''), [usera])

        self.assertEqual(await client.query(r'''
            users.filter(
                |user| isnil(
                    user.memberships.find(
                        |membership| (
                            membership.workspace == workspace
                        )
                    ).channels.indexof(channel)
                )
            );
        '''), [userb])


if __name__ == '__main__':
    run_test(TestNested())