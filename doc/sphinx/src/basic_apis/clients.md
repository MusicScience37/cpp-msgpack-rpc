# Clients

## Simple Example

A {cpp:class}`msgpack_rpc::clients::Client` object can be created
using {cpp:class}`msgpack_rpc::clients::ClientBuilder`.
After creating a client,
{cpp:func}`msgpack_rpc::clients::Client::call` function
can be used to call a method.

```{literalinclude} ../../../../examples/simple/simple_client.cpp
:caption: Example of a Simple Client
:language: cpp
:lines: 20-
```

## APIs of Clients

```{doxygenclass} msgpack_rpc::clients::ClientBuilder

```

```{doxygenclass} msgpack_rpc::clients::Client

```

```{doxygenclass} msgpack_rpc::clients::CallFuture

```

```{doxygenclass} msgpack_rpc::clients::ServerException

```
