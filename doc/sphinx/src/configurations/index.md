(configuration_files)=

# Configuration Files

Configurations of loggers, clients and servers can be written in configuration files,
and parsed using {cpp:class}`msgpack_rpc::config::ConfigParser`.

## Schemas

```{toctree}
:maxdepth: 1
:caption: JSON Schemas

cpp-msgpack-rpc configuration schema (latest) <https://cppmsgpackrpc.musicscience37.com/schemas/config_schema.json>
cpp-msgpack-rpc configuration schema version 1 <https://cppmsgpackrpc.musicscience37.com/schemas/config_schema_v1.json>
```

```{toctree}
:maxdepth: 1
:caption: Documentations

schemas/config_schema_v1
```

## Configuration Parameters

Here shows configuration parameters.
All parameters can be omitted,
and the default values of the parameters are as follows:

```{literalinclude} ../../../../examples/configs/default_config.toml
:caption: Default configurations of cpp-msgpack-rpc
:language: toml
```

When no configuration file is used,
the defaults in the above configurations are used.

Also, empty configurations like following can be used:

```{literalinclude} ../../../../examples/configs/empty_config.toml
:caption: Empty configurations
:language: toml
```
