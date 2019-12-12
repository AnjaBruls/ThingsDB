import time
import asyncio
import logging
import pprint
import pickle
import signal
import ssl
from thingsdb.client import Client
from thingsdb.exceptions import ThingsDBError
from thingsdb.exceptions import LookupError
from thingsdb.exceptions import NodeError


interrupted = False


async def test(client):
    global osdata

    await client.connect('35.204.223.30', port=9400)
    await client.authenticate('aoaOPzCZ1y+/f0S/jL1DUB')  # admin
    # await client.authenticate('V1CsgMetJcOHlqPGCigitz')  # Kolnilia

    client.use('Kolnilia')

    try:
        res = await client.query('''
            .greet;
        ''')

        pprint.pprint(res)

        res = await client.query('''
            procedures_info();
        ''', scope='@t')

        pprint.pprint(res)

        # res = await client.query('''
        #     nodes_info();
        # ''', scope='@n')
        # pprint.pprint(res)

        # res = await client.run('new_playground', 'Kolnilia', scope='@t')
        # pprint.pprint(res)

    finally:
        client.close()


def signal_handler(signal, frame):
    print('Quit...')
    global interrupted
    interrupted = True


if __name__ == '__main__':
    client = Client(ssl=ssl.SSLContext(ssl.PROTOCOL_TLS))
    signal.signal(signal.SIGINT, signal_handler)

    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    loop = asyncio.get_event_loop()
    loop.run_until_complete(test(client))
    loop.run_until_complete(client.wait_closed())
    print('-----------------------------------------------------------------')
