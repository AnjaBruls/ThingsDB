# ThingsDB

## TODO / Road map

- [x] Improve syntax errors
- [ ] Update documentation
  - [ ] Slices doc
  - [ ] Update parenthesis which are not required anymore
  - [ ] Add procedure docs
  - [ ] Update ternary operator
- [ ] Start project **ThingsBook** for a beginner guide on how to work with ThingsDB
- [x] Review exceptions raised bt ThingsDB. (maybe add some additional ones?)
- [x] Introduce a WARNING protocol which will be send to client as fire-and-forget
      Maybe in the format `{"warn_msg": ... "warn_code": }` ?
      Somewhere it should be possible to disable warning messages. This could be
      done with a client protocol, or change the auth protocol? or maybe it should
      be disabled explicitly for a query. This last option is maybe not so bad
      because it will prevent missing warnings.
- [x] Some sort of `Type` support for strict types?
  - [ ] Create a `cast` type for exporting less data


## Plans and Ideas for the Future
- [ ] Big number support?



## Get status

```
wget -q -O - http://node.local:8080/status
```

## Special thanks to:

 - [Fast Validate UTF-8](https://github.com/lemire/fastvalidate-utf-8)

## Fonts:

https://fonts.adobe.com/fonts/keraleeyam
