import asyncio
import logging
from .package import Package
from .exceptions import MaxQuotaError
from .exceptions import AuthError
from .exceptions import ForbiddenError
from .exceptions import IndexError
from .exceptions import BadRequestError
from .exceptions import QueryError
from .exceptions import NodeError
from .exceptions import InternalError


REQ_PING = 32
REQ_AUTH = 33
REQ_QUERY = 34

RES_PING = 64
RES_AUTH = 65
RES_QUERY = 66

RES_ERR_MAX_QUOTA = 96
RES_ERR_AUTH = 97
RES_ERR_FORBIDDEN = 98
RES_ERR_INDEX = 99
RES_ERR_BAD_REQUEST = 100
RES_ERR_QUERY = 101
RES_ERR_NODE = 102
RES_ERR_INTERNAL = 103


PROTOMAP = {
    RES_PING: lambda f, d: f.set_result(None),
    RES_AUTH: lambda f, d: f.set_result(None),
    RES_QUERY: lambda f, d: f.set_result(d),
    RES_ERR_MAX_QUOTA:
        lambda f, d: f.set_exception(MaxQuotaError(errdata=d)),
    RES_ERR_AUTH:
        lambda f, d: f.set_exception(AuthError(errdata=d)),
    RES_ERR_FORBIDDEN:
        lambda f, d: f.set_exception(ForbiddenError(errdata=d)),
    RES_ERR_INDEX:
        lambda f, d: f.set_exception(IndexError(errdata=d)),
    RES_ERR_BAD_REQUEST:
        lambda f, d: f.set_exception(BadRequestError(errdata=d)),
    RES_ERR_QUERY:
        lambda f, d: f.set_exception(QueryError(errdata=d)),
    RES_ERR_NODE:
        lambda f, d: f.set_exception(NodeError(errdata=d)),
    RES_ERR_INTERNAL:
        lambda f, d: f.set_exception(InternalError(errdata=d)),
}


def proto_unkown(f, d):
    f.set_exception(TypeError('unknown package type received ({})'.format(d)))


class Protocol(asyncio.Protocol):

    _connected = False

    def __init__(self):
        self._buffered_data = bytearray()
        self.package = None

    def connection_made(self, transport):
        '''
        override asyncio.Protocol
        '''
        pass

    def connection_lost(self, exc):
        '''
        override asyncio.Protocol
        '''
        pass

    def data_received(self, data):
        '''
        override asyncio.Protocol
        '''
        self._buffered_data.extend(data)
        while self._buffered_data:
            size = len(self._buffered_data)
            if self.package is None:
                if size < Package.struct_datapackage.size:
                    return None
                self.package = Package(self._buffered_data)
            if size < self.package.total:
                return None
            try:
                self.package.extract_data_from(self._buffered_data)
            except KeyError as e:
                logging.error('Unsupported package received: {}'.format(e))
            except Exception as e:
                logging.exception(e)
                # empty the byte-array to recover from this error
                self._buffered_data.clear()
            else:
                self.on_package_received(self.package)
            self.package = None

    @staticmethod
    def on_package_received(pkg):
        raise NotImplementedError