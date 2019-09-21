/*
 * ti/type.h
 */
#ifndef TI_TYPE_H_
#define TI_TYPE_H_

typedef struct ti_type_s ti_type_t;

#include <ti/types.h>

ti_type_t * ti_type_create(
        ti_types_t * types,
        uint16_t id,
        const char * name,
        size_t n);
void ti_type_destroy(ti_type_t * type);


struct ti_type_s
{
    uint32_t ref;
    uint16_t class;         /* type id */
    char * name;            /* name (null terminated) */
    uint32_t name_n;
    vec_t * dependencies;   /* ti_type_t with reference */
    vec_t * fields;         /* ti_field_t */
};

#endif  /* TI_TYPE_H_ */



/*

// 'str'   -> do utf8 check
// 'raw'   -> like str, without utf8 check
// 'int'   -> integer value
// 'float' -> float value
// 'bool'  -> bool value
// 'thing' -> thing
// '[]'    -> array
// '{}'    -> set
// '[...]`  -> array of ...
// '{...}`  -> set of ...


define('User', {});


kind_info('Channel');
new_kind('Channel');

del_kind('Channel');  // marks as deleted immediately,
                         convert to things in away mode





define('Channel', {
    name: 'str',          // defaults to ""
    description: 'str?',  // defaults to nil
    parent: 'Channel?',   // self references must be optional or inside an array
    users: '[User]',
});

define('UserName', {
    name: 'str'
})

define('User', {
    name: 'str',
    age: 'int'
    channel: 'Channel'
});

define('ChannelCompact' {
    name: 'str',
    users: '[UserName]'
})

// event `define` is only handled by the nodes, so for updating watchers
// we should generate `set` events and write them immediately while processing
// the change. A difference is that we only require the set events when there
// are actually watchers

channel = new('Channel', {})
            -->
            {
                #: 123
                name: "",
                description: nil,
                parent: nil,
                users: [],
            }

channel.x = 1;          // error, type `Channel` has no property `x`
channel.name = 1;       // error, type `Channel` expects `raw`...

user = new('User', {})
user.name = 'Iris';

as_channel = wrap('Channel', user);   // create short syntax <Channel>user;
as_channel.description;    //  return default, nil? or error? and what in return?

as_channel.name = 'xxx';    // error, cannot assign to <Channel>

type(as_channel);           // <Channel>
type(user);                 // User

user = unwrap(as_channel);  // returns User //


// what should a double cast do? wrap('X', wrap('Y', value)); ?
// replace the wrap or allow double wrapping? If returning default

// internally we can pack a wrap as {'&': kind, object: ...} but we expose
// the wrapped object as it is cast.

*/