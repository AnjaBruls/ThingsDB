import asyncio
import logging
import random
import ssl
from typing import Optional, Union, Any
from deprecation import deprecated
from .buildin import Buildin
from .protocol import Proto
from .protocol import Protocol
from ..convert import convert
from .abc.events import Events


class Client(Buildin):

    MAX_RECONNECT_WAIT_TIME = 120
    MAX_RECONNECT_TIMEOUT = 10

    def __init__(
            self,
            auto_reconnect: bool = True,
            ssl: Optional[Union[bool, ssl.SSLContext]] = None,
            loop: Optional[asyncio.AbstractEventLoop] = None,
    ) -> None:
        """Initialize a ThingsDB client.

        Args:
            auto_reconnect (bool, optional):
                When set to `True`, the client will automatically
                reconnect when a connection is lost. If set to `False` and the
                connection gets lost, one may call the `reconnect()` method to
                make a new connection. Defaults to True.
            ssl (SSLContext or bool, optional):
                Accepts an ssl.SSLContext for creating a secure connection
                using SSL/TLS. This argument may simply be set to `True` in
                which case a context using `ssl.PROTOCOL_TLS` is created.
                Defaults to None.
            loop (AbstractEventLoop, optional):
                Can be used to run the client on a specific event loop.
                If this argument is not used, the default event loop will be
                used. Defaults to None.
        """

        self._loop = loop if loop else asyncio.get_event_loop()
        self._auth = None
        self._pool = None
        self._protocol = None
        self._pid = 0
        self._reconnect = auto_reconnect
        self._scope = '@t'  # default to thingsdb scope
        self._pool_idx = 0
        self._reconnecting = False
        if ssl is True:
            self._ssl = ssl.SSLContext(ssl.PROTOCOL_TLS)
        elif ssl is False:
            self._ssl = None
        else:
            self._ssl = ssl
        self._event_handlers = []

    def add_event_handler(self, event_handler: Events) -> None:
        """Add an event handler.

        Event handlers will called in the order they are added.

        Args:
            event_handler (Events):
                An instance of Events (see thingsdb.client.abc.events).
        """
        self._event_handlers.append(event_handler)

    def get_event_loop(self) -> asyncio.AbstractEventLoop:
        """Can be used to get the event loop.

        Returns:
            AbstractEventLoop: The event loop used by the client.
        """
        return self._loop

    def is_connected(self) -> bool:
        """Can be used to check if the client is conected.

        Returns:
            bool: True when the client is connected else False.
        """
        return bool(self._protocol and self._protocol.transport)

    def set_default_scope(self, scope: str) -> None:
        """Set the default scope.

        Can be used to change the default scope which is initially set to `@t`.

        Args:
            scope (str):
                Set the default scope. A scope may start with either the `/`
                character, or `@`. Examples: "//stuff", "@:stuff", "/node"
        """
        assert scope.startswith('@') or scope.startswith('/')
        self._scope = scope

    def get_default_scope(self) -> str:
        """Get the default scope.

        The default scope may be changed with the `set_default_scope()` method.

        Returns:
            str:
                The default scope which is used by the client when no specific
                scope is specified.
        """
        return self._scope

    def close(self) -> None:
        """Close the ThingsDB connection.

        This method will return immediately so the connection may not be
        closed yet after a call to `close()`. Use the `wait_closed()` method
        after calling this method if this is required.
        """
        if self._protocol and self._protocol.transport:
            self._reconnect = False
            self._protocol.transport.close()

    def connection_info(self) -> str:
        """Returns the current connection info as a string.

        Even with a connection pool, the client has still one active node
        connection at the time, and info for this active connection will be
        returned.

        example: "node0.local:9200"
        """
        if not self.is_connected():
            return 'disconnected'
        socket = self._protocol.transport.get_extra_info('socket', None)
        if socket is None:
            return 'unknown_addr'
        addr, port = socket.getpeername()
        return f'{addr}:{port}'

    async def connect_pool(self, pool: list, *auth: Union[str, tuple]) -> None:
        """Connect using a connection pool.

        When using a connection pool, the client will randomly choose a node
        to connect to. When a node is going down, it will inform the client
        so it will automitically re-connect to another node. Connections will
        automatically authenticate so the connection pool requires credentials
        to perfrom the authentication.

        Examples:
            >>> await connect_pool([
                'node01.local',             # address as string
                'node02.local',             # port will default to 9200
                ('node03.local', 9201),     # ..or with an eplicit port
            ], "admin", "pass")

        Args:
            pool (list of addresses):
                Should be an iterable with node address strings, or tuples
                with `address` and `port` combinations in a tuple or list.
            *auth (str or (str, str)):
                Argument `auth` can be be either a string with a token or a
                tuple with username and password. (the latter may be provided
                as two separate arguments

        Remarks:
            Do not use this method if the client is already
            connected. This can be checked with `client.is_connected()`.
        """
        assert self.is_connected() is False
        if len(auth) == 1:
            auth = auth[0]

        self._pool = tuple((
            (address, 9200) if isinstance(address, str) else address
            for address in pool))
        self._auth = self._auth_check(auth)
        self._pool_idx = random.randint(0, len(pool) - 1)
        await self.reconnect()

    async def connect(
            self,
            host: str,
            port: int = 9200,
            timeout: int = 5
    ) -> None:
        """Connect to ThingsDB.

        This method will *only* create a connection, so the connection is not
        authenticated yet. Use the `authenticate(..)` method after creating a
        connection before using the connection.

        Args:
            host (str):
                A hostname, IP address, FQDN to connect to.
            port (int, optional):
                Integer value between 0 and 65535 and should be the port number
                where a ThingsDB node is listening to for client connections.
                Defaults to 9200.
            timeout (int, optional):
                Can be be used to control the maximum time the client will
                attempt to create a connection. Defaults to 5.

        Remarks:
            Do not use this method if the client is already
            connected. This can be checked with `client.is_connected()`.
        """
        assert self.is_connected() is False
        self._pool = ((host, port),)
        self._pool_idx = 0
        await self._connect(timeout=timeout)

    async def reconnect(self) -> None:
        """Re-connect to ThingsDB.

        This method can be used, even when a connection still exists. In case
        of a connection pool, a call to `reconnect()` will switch to another
        node.
        """
        if self._reconnecting:
            return

        self._reconnecting = True
        try:
            await self._reconnect_loop()
        finally:
            self._reconnecting = False

    async def wait_closed(self) -> None:
        """Wait for a connecion to close.

        Can be used after calliing the `close()` method to determine when the
        connection is actually closed.
        """
        if self._protocol and self._protocol.close_future:
            await self._protocol.close_future

    async def authenticate(self, *auth, timeout: int = 5) -> None:
        """Authenticate a ThingsDB connection.

        Args:
            *auth (str or (str, str)):
                Argument `auth` can be be either a string with a token or a
                tuple with username and password. (the latter may be provided
                as two separate arguments
            timeout (int, optional):
                Can be be used to control the maximum time in seconds for the
                client to wait for response on the authentication request.
                Defaults to 5.
        """
        if len(auth) == 1:
            auth = auth[0]
        self._auth = self._auth_check(auth)
        await self._authenticate(timeout)

        if self._reconnect:
            await self.watch(scope='@n')

    async def query(
            self,
            code: str,
            scope: Optional[str] = None,
            timeout: Optional[int] = None,
            convert_args: Optional[bool] = True,
            **kwargs: Any
    ) -> Any:
        if self._protocol is None:
            raise ConnectionError('no connection')

        if scope is None:
            scope = self._scope

        if kwargs:
            if convert_args:
                kwargs = {k: convert(v) for k, v in kwargs.items()}
            data = [scope, code, kwargs]
        else:
            data = [scope, code]

        future = self._protocol.write(Proto.REQ_QUERY, data, timeout=timeout)
        return await future

    async def run(self, procedure: str, *args, scope=None, convert_args=True):
        if self._protocol is None:
            raise ConnectionError('no connection')

        if scope is None:
            scope = self._scope

        arguments = (convert(arg) for arg in args) if convert_args else args

        future = self._protocol.write(
            Proto.REQ_RUN,
            [scope, procedure, *arguments],
            timeout=None)

        return await future

    def watch(self, *ids: int, scope: Optional[str] = None):
        if self._protocol is None:
            raise ConnectionError('no connection')

        if scope is None:
            scope = self._scope

        return self._protocol.write(Proto.REQ_WATCH, [scope, *ids])

    def unwatch(self, *ids: int, scope: Optional[str] = None):
        if self._protocol is None:
            raise ConnectionError('no connection')

        if scope is None:
            scope = self._scope

        return self._protocol.write(Proto.REQ_UNWATCH, [scope, *ids])

    @staticmethod
    def _auth_check(auth):
        assert ((
            isinstance(auth, (list, tuple)) and
            len(auth) == 2 and
            isinstance(auth[0], str) and
            isinstance(auth[1], str)
        ) or (
            isinstance(auth, str)
        )), (
            'expecting the authentication argument to be a "token-string" '
            'or a tuple like ("username", "password").'
        )
        return auth

    async def _connect(self, timeout=5):
        host, port = self._pool[self._pool_idx]
        try:
            conn = self._loop.create_connection(
                lambda: Protocol(
                    on_connection_lost=self._on_connection_lost,
                    on_event=self._on_event,
                    loop=self._loop),
                host=host,
                port=port,
                ssl=self._ssl)
            _, self._protocol = await asyncio.wait_for(
                conn,
                timeout=timeout)
        finally:
            self._pool_idx += 1
            self._pool_idx %= len(self._pool)

    def _on_event(self, pkg):
        if pkg.tp == Proto.ON_NODE_STATUS and \
                self._reconnect and \
                pkg.data == 'SHUTTING_DOWN':
            asyncio.ensure_future(self.reconnect(), loop=self._loop)

        for event_handler in self._event_handlers:
            event_handler(pkg.tp, pkg.data)

    def _on_connection_lost(self, protocol, exc):
        if self._protocol is not protocol:
            return

        self._protocol = None

        if self._reconnect:
            asyncio.ensure_future(self.reconnect(), loop=self._loop)

    async def _reconnect_loop(self):
        wait_time = 1
        timeout = 2
        protocol = self._protocol
        while True:
            host, port = self._pool[self._pool_idx]
            try:
                await self._connect(timeout=timeout)
            except Exception as e:
                logging.error(
                    f'connecting to {host}:{port} failed ({e}), '
                    f'try next connect in {wait_time} seconds'
                )
            else:
                if protocol and protocol.transport:
                    # make sure the `old` connection will be dropped
                    self._loop.call_later(10.0, protocol.transport.close)
                break

            await asyncio.sleep(wait_time)
            wait_time *= 2
            wait_time = min(wait_time, self.MAX_RECONNECT_WAIT_TIME)
            timeout = min(timeout+1, self.MAX_RECONNECT_TIMEOUT)

        await self._authenticate(timeout=5)

        if self._reconnect:
            await self.watch(scope='@n')

        for event_handler in self._event_handlers:
            event_handler.on_reconnect()

    async def _authenticate(self, timeout):
        if self._protocol is None:
            raise ConnectionError('no connection')

        await self._protocol.write(
            Proto.REQ_AUTH,
            data=self._auth,
            timeout=timeout)

    @deprecated(details='Use `set_default_scope` instead')
    def use(self, scope):
        if not scope.startswith('@'):
            scope = f'@{scope}' if scope.startswith(':') else f'@:{scope}'
        self.set_default_scope(scope)

    @deprecated(details='Use `get_default_scope` instead')
    def get_scope(self):
        return self._scope
