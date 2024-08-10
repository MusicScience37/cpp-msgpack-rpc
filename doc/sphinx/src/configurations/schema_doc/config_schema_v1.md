# cpp-msgpack-rpc configuration schema version 1

*Schema of configuration files in cpp-msgpack-rpc library.*

## Properties

- **`logging`** *(object)*: Configurations of loggers. This is a mapping from configuration names to the configurations of loggers. Cannot contain additional properties.
  - **`.+`** *(object)*: Configurations of loggers. Cannot contain additional properties.
    - **`filepath`** *(string)*: Path of the log file. Empty filepath specifies output to console.
    - **`max_file_size`** *(integer)*: Maximum size of a file in bytes. Minimum: `1`.
    - **`max_files`** *(integer)*: Maximum number of files. Minimum: `1`.
    - **`output_log_level`** *(string)*: Minimum log level to write logs. Must be one of: `["trace", "debug", "info", "warn", "error", "critical"]`.
- **`client`** *(object)*: Cannot contain additional properties.
  - **`.+`** *(object)*
- **`server`** *(object)*: Cannot contain additional properties.
  - **`.+`** *(object)*
