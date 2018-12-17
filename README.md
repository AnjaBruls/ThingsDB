# ThingsDB

## TODO list

- [x] Take advantage when on a 64 bit system
    - [x] wareq uses uinpntr_t for 64bit thing id's, on 32 bit id's are allocated
    - [x] pre-caching is done on 64bit systems for integer and float values
- [x] Special floats
    - [x] Inf
    - [x] -Inf
        - [x] Add `isinf` function for checking
    - [x] NaN
        - [x] Add `isnan` function for checking
    - [x] e+- notation
- [x] Support other base integers
    - [x] 0x for hex
    - [x] 0o for octal
    - [x] 0b for binary
- [ ] ~~We can make `indexing by scope` instead of using a fixed integer, but
      allowing only fixed integers makes it a bit faster and easier~~
      We can always do this later since `index by scope` is backwards compatible with the
      current syntax, but not the other way around.
- [ ] ~~Overflow handling? Right now ThingsDB is naive~~
      We could check ERANGE after investigating since nothing else can set
      ERANGE during investigation. The other checks must be done in ti_opr.
      - [ ] Introduce big numbers, export as hex {-:''} and {+:''}
      - [ ] bit_t
        - [x] `big_to_str16n`
        - [x] `big_null`
        - [x] `big_from_int64`
        - [x] `big_to_int64`
        - [ ] `big_from_double`
        - [ ] `big_to_double`
        - [x] `big_mulii` -> big
        - [x] `big_mulbb` -> big
        - [x] `big_mulbi` -> big
        - [ ] `big_mulbd` -> double
        - [ ] `big_addii` -> big
        - [ ] `big_addbb` -> big
        - [ ] `big_addbi` -> big
        - [ ] `big_addbd` -> double
        - [ ] `big_subii` -> big
        - [ ] `big_subbb` -> big
        - [ ] `big_subbi` -> big
        - [ ] `big_subbd` -> double
        - [ ] `big_divii` -> double
        - [ ] `big_divbb` -> double
        - [ ] `big_divbi` -> double
        - [ ] `big_divbd` -> double
        - [ ] `big_idivii` -> big
        - [ ] `big_idivbb` -> big
        - [ ] `big_idivbi` -> big
        - [ ] `big_idivbd` -> big
        - [ ] `big_modii`
        - [ ] `big_modbb`
        - [ ] `big_modbi`
        - [ ] `big_andbi`
        - [ ] `big_andbb`
        - [ ] `big_orbi`
        - [ ] `big_orbb`
        - [ ] `big_xorbi`
        - [ ] `big_xorbb`
        - [ ] `big_eqbb`
        - [x] `big_is_positive`
        - [x] `big_is_negative`
        - [x] `big_str16_msize`
        - [x] `big_fits_int64`
        - [x] `big_is_null`
- [x] Refactor
    - [x] database -> collection
    - [x] user_new etc -> new_user
    - [x] ti_res_t & ti_root_t  -> ti_query_t
    - [x] watch request only on collections (target to ->collection)
- [x] Watching
    - [x] watch
    - [x] unwatch
- [ ] Language
    - [x] Primitives
        - [x] `false`
        - [x] `nil`
        - [x] `true`
        - [x] `float`
        - [x] `int`
        - [x] `string`
        - [x] `regex`
    - [x] Thing
    - [x] Array
    - [ ] Functions:
        - [x] `blob`
            - [x] array implementation
            - [ ] ~~Future feature: map implementation~~
        - [x] `endswith`
        - [x] `filter`
        - [ ] `find`
        - [x] `get`
        - [x] `isinf`
        - [x] `isnan`
        - [x] `id`
        - [x] `lower`
        - [x] `map`
        - [x] `match`
        - [x] `now`
        - [x] `ret`
        - [x] `startswith`
        - [x] `thing`
        - [x] `upper`
        - [x] `del`
        - [x] `push`
        - [x] `rename`
        - [x] `set`
        - [x] `splice`
        - [x] `unset`
- [x] Redundancy as initial argument together with --init ?
- [ ] Task generating
    - [x] `assign`
    - [x] `del`
    - [x] `push`
    - [x] `rename`
    - [x] `set`
    - [x] `splice`
    - [x] `unset`
- [x] Events without tasks could be saved smaller
- [ ] Jobs processing from `EPKG`
    - [x] `assign`
    - [x] `del`
    - [x] `push`
    - [x] `rename`
    - [x] `set`
    - [x] `splice`
    - [x] `unset`
- [x] Value implementation
    - [x] Bool
    - [x] Int
    - [x] Float
    - [x] Nil
    - [x] Thing
    - [x] Array
    - [x] Tuple
    - [x] Things
    - [x] Regex
    - [x] Arrow
    - [x] Raw
    - [x] Qp
- [x] Operations
    - [x] `eq`
    - [x] `ge`
    - [x] `gt`
    - [x] `le`
    - [x] `lt`
    - [x] `ne`
    - [x] `add`
    - [x] `sub`
    - [x] `mul`
    - [x] `div`
    - [x] `idiv`
    - [x] `mod`
    - [x] `b_and`
    - [x] `b_xor`
    - [x] `b_or`
- [x] Expressions
    - [x] AND
    - [x] OR
    - [x] Parenthesis
- [x] Keep lowest stored event_id on all nodes
- [x] Implement collection quotas
    - [x] max things
    - [x] max array size
    - [x] max raw size
    - [x] max properties
- [x] Build first event on init
- [x] Storing
    - [x] Full storage on disk
        - [x] Status
        - [x] Nodes
        - [x] Databases
        - [x] Access
        - [x] Things
            - [x] Skeleton
            - [x] Data
            - [x] Attributes
        - [x] Users
    - [x] Archive storing
        - [x] Store in away mode
        - [x] Load on startup (Required Jobs implementation for full coverage)
- [x] Remove *This* node id from `ti_.qp` file and use a separate file because this
      is the only value which is not the same over all nodes.
- [ ] Multi node
    - [ ] Design flow
    - [ ] Zone info (so we can first try a node in the same zone for forward queries)
    - [x] Lookup Should not be a singleton so we can create a desired lookup.
    - [x] Each node should have a secret
        - [x] Secrets should be graphical only. (check on argument input and query input)
        - [x] Secrets must be stored (and restored)
        - [x] On --init, the first node should create a *random* secret.
    - [ ] Create a request for the ThingsDB setup. (data in `ti_.qp`)
    - [ ] Add node
        - [ ] In cq -> parse address:port and secret
        - [ ] In Node, connect to new node
            - [ ] Protocol send node_id and secret (using crypt?)
                - Will it work if we send a node_id to replace an existing
                  node? In this case we can keep the argument --init and do
                  not need an extra argument for replacing an existing node
            - [ ] Send all nodes to new node
                - [ ] On callback -> Add the node and register a task for the
                      other nodes. Now all nodes can connect to the new node.
                      The new node should not set-up connections.
                - [ ] On shutdown and restart, the event makes sure the node is
                      still registered.
                - [ ] nope, stupid idea --> What if we store the 'known' nodes length, and
                      only after finished expanding update the length for the
                      lookup?
                - [ ] I think this works --> What if we store node flags (SYNCHRONIZING) ? But also
                      keep the status SYNCHRONIZING which is based on the flag.
                      The status can be used for communicating, and the flag for
                      hard storage. ( we only need the flag on the 'new' node) -->
                      not true, we should store the flag on all nodes, and also add
                      a dropped flag for scaling down. Scaling down is then possible
                      in case we scale down by one node at a time. I think we
                      should only allow to pop the last node, since otherwise
                      we need to change node id's to change the place of the
                      removed node.

                - [ ] What if we add another node while synchronizing is not
                      finished yet? As long as the redundancy is not changed,
                      it should work since the new nodes only can request too
                      much attributes which later can be removed.
                      (but never too little, and this is not the case if the
                      redundancy setting changes)
                      - Solution to support change of redundancy: say we have state A,
                        and create a desired state B and keep
                        attributes for both state A and B. If state B is replaced with
                        state C, then we can forget about state B, and continue saving
                        state A and C. As soon as the higher state is finished we can
                        forget about state A.



            - [ ] Connect to all node
            - [ ] Search away mode node
            - [ ] Request everything stored except attributes
            - [ ] Request archived events
            - [ ]
        - [ ] In nodes -> ti_nodes_new_node function
- [ ] Root
    - [ ] functions
        - [x] `collection`
        - [x] `collections`
        - [x] `counters`
        - [x] `del_collection`
            - [x] Make sure dropped collections will be garbage collected while in away mode
        - [ ] `pop_node` --> pop so we do not need to replace node id's
        - [x] `del_user`
        - [x] `grant`
        - [x] `new_collection`
        - [ ] `new_node`
        - [x] `new_user`
        - [x] `node`
        - [x] `nodes`
        - [ ] `rename_user`
        - [ ] `rename_collection`
        - [x] `reset_counters`
        - [x] `revoke`
        - [x] `set_loglevel`
        - [ ] `set_password`
        - [x] `set_quota`
        - [x] `set_zone`
        - [x] `shutdown`
        - [x] `user`
        - [x] `users`
    - [ ] jobs
        - [x] `del_collection`
        - [ ] `pop_node`
            - [ ] We should check the current 'lookup' state and only allow to
                  pop when having at least (lookup_n - lookup_r + 2) nodes.
        - [x] `del_user`
        - [x] `grant`
        - [x] `new_collection`
        - [ ] `new_node`  (address:port, secret)
        - [ ] `replace_node`
        - [x] `new_user`
        - [x] `revoke`
        - [ ] `rename_user`
        - [ ] `rename_collection`
        - [ ] `set_password`
        - [x] `set_quota`


## Flow for adding a new node

1. First the query `new_node('secret', 'ip-address' [, port])` is used to add a
   new node and requires an event.
   The 3th argument, port, is optional and falls back to the default
   if not given.
   An ip address (2nd argument) is required and I think we should not support
   dns names but only IPv4 and IPv6 addresses. Reason why:
    - dns names are hard to check for "wrong" input while processing the query
    - a lookup can only be performed *after* the query, out-side the event since
      this must be an asynchronous call.
    - the risk of one node being able to connect while another node is not,
      becomes larger since dns is just another *layer* which might fail.

   If *invalid* input is given, nothing is done (no task for the event is created)

2. On successful input, a task is created containing the `secret`, `node_id`, and `socket-address`.
   The node will be added to the nodes list (and flagged PENDING? on a second thought, the PENDING flag is maybe not required)
   ThingsDB state will be saved to disk immediately? ->This is actually not really required
   The root job processor should check if the node is already saved, and otherwise skip adding
   the node. (therefore we really need the `node_id`)
