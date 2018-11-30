# ThingsDB

## TODO list

- [ ] Language
    - [ ] Primitives
        - [x] `false`
        - [x] `nil`
        - [x] `true`
        - [x] `float`
        - [x] `int`
        - [x] `string`
        - [x] `regex`
        - [x] `undefined`
        - [ ] Introduce a Date type ?
    - [x] Thing
    - [ ] Array
    - [ ] Functions:
        - [ ] `blob`
            - [x] array implementation
            - [ ] map implementation
        - [x] `endswith`
        - [x] `filter`
        - [x] `get`
        - [x] `id`
        - [x] `lower`
        - [x] `map`
        - [x] `match`
        - [x] `ret`
        - [x] `startswith`
        - [x] `thing`
        - [x] `upper`
        - [x] `del`
        - [x] `push`
        - [ ] `rename`
        - [x] `set`
        - [ ] `splice`
        - [x] `unset`
- [ ] Redundancy as initial argument together with --init ?
- [ ] Task generating
    - [x] `assign`
    - [x] `del`
    - [x] `push`
    - [ ] `rename`
    - [x] `set`
    - [ ] `splice`
    - [x] `unset`
- [ ] Jobs processing from `EPKG`
    - [ ] `assign`
    - [ ] `del`
    - [ ] `push`
    - [ ] `rename`
    - [ ] `set`
    - [ ] `splice`
    - [ ] `unset`
- [ ] Value implementation
    - [x] Bool
    - [x] Int
    - [x] Float
    - [x] Nil
    - [x] Thing
    - [x] Array
    - [x] Tuple
    - [x] Things
- [ ] Storing
    - [ ] Full storage on disk
        - [x] Status
        - [x] Nodes
        - [x] Databases
        - [x] Access
        - [ ] Things
            - [x] Skeleton
            - [ ] Data
            - [ ] Attributes
        - [x] Users
            - [ ] Additional user properties like email ?
    - [x] Archive storing
        - [x] Store in away mode
        - [x] Load on startup (Required Jobs implementation for full coverage)
- [ ] Multi node
- [ ] Root functions
    - [ ] Create a way to perform root function, maybe all commands with
          functions ?