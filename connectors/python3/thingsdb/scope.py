from .protocol import REQ_QUERY_COLLECTION
from .protocol import REQ_QUERY_NODE
from .protocol import REQ_QUERY_THINGSDB


class Scope:

    def __init__(self, scope: str, _proto=REQ_QUERY_COLLECTION):
        self._scope = scope
        self._proto = _proto

    def is_collection(self) -> bool:
        return self._proto == REQ_QUERY_COLLECTION

    def name(self):
        if isinstance(self._scope, int):
            return f'collection:{self._scope}'
        return self._scope


ThingsDBScope = Scope('scope:thingsdb', _proto=REQ_QUERY_THINGSDB)
NodeScope = Scope('scope:node', _proto=REQ_QUERY_NODE)