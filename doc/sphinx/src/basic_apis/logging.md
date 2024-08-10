# Logging

For logging, the following logging macros are provided:

- `MSGPACK_RPC_TRACE`
- `MSGPACK_RPC_DEBUG`
- `MSGPACK_RPC_INFO`
- `MSGPACK_RPC_WARN`
- `MSGPACK_RPC_ERROR`
- `MSGPACK_RPC_CRITICAL`

These can be used with {cpp:class}`msgpack_rpc::logging::Logger` class.

{cpp:class}`msgpack_rpc::logging::Logger` objects can be created using
{cpp:func}`msgpack_rpc::logging::Logger::create` function.
Examples to create loggers can be found in

- [More APIs in Clients](./clients.md#more-apis)
- [More APIs in Servers](./servers.md#more-apis)

## APIs of Logging

```{doxygendefine} MSGPACK_RPC_TRACE

```

```{doxygendefine} MSGPACK_RPC_DEBUG

```

```{doxygendefine} MSGPACK_RPC_INFO

```

```{doxygendefine} MSGPACK_RPC_WARN

```

```{doxygendefine} MSGPACK_RPC_ERROR

```

```{doxygendefine} MSGPACK_RPC_CRITICAL

```

```{doxygenclass} msgpack_rpc::logging::Logger

```

```{doxygenenum} msgpack_rpc::logging::LogLevel

```

```{doxygenclass} msgpack_rpc::logging::ILogSink

```

```{doxygenclass} msgpack_rpc::logging::SourceLocationView

```

```{doxygendefine} MSGPACK_RPC_CURRENT_SOURCE_LOCATION

```
