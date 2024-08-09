# Servers

## Simple Example

A {cpp:class}`msgpack_rpc::servers::Server` object can be created
using {cpp:class}`msgpack_rpc::servers::ServerBuilder`.
After creating a client,
{cpp:func}`msgpack_rpc::servers::Server::run_until_signal` function
can be used to run the server until SIGINT or SIGTERM is received.

```{literalinclude} ../../../../examples/simple/simple_server.cpp
:caption: Example of a Simple Server
:language: cpp
:lines: 20-
```

## APIs of Servers

```{doxygenclass} msgpack_rpc::servers::ServerBuilder

```

```{doxygenclass} msgpack_rpc::servers::Server

```

```{doxygenclass} msgpack_rpc::methods::MethodException

```
