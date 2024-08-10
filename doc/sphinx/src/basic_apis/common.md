# Common

Here shows some common classes and enumerations in this library.

## Exceptions

This library has following exception types:

- {cpp:class}`msgpack_rpc::MsgpackRPCException`
  - General Exceptions in this library.
  - Errors can be stored in {cpp:class}`msgpack_rpc::Status`
    with status codes given by {cpp:enum}`msgpack_rpc::common::StatusCode`.
- {cpp:class}`msgpack_rpc::clients::ServerException`
  - Used only in clients.
  - For errors in a server to which a client sent a request.
- {cpp:class}`msgpack_rpc::methods::MethodException`
  - Used only in servers.
  - Used in implementations of methods in servers
    to specify errors in the method with serializable objects.

## URIs

URIs can be processed using
{cpp:class}`msgpack_rpc::addresses::URI`.

## APIs of Common

```{doxygenclass} msgpack_rpc::common::MsgpackRPCException

```

```{doxygenclass} msgpack_rpc::common::Status

```

```{doxygenenum} msgpack_rpc::common::StatusCode

```

```{doxygenclass} msgpack_rpc::addresses::URI

```
