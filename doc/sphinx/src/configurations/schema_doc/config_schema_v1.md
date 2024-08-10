# cpp-msgpack-rpc configuration schema version 1

*Schema of configuration files in cpp-msgpack-rpc library.*

## Properties

- **`logging`** *(object)*: Configurations of loggers. This is a mapping from configuration names to the configurations of loggers. Cannot contain additional properties.
  - **`.+`** *(object)*: Configurations of loggers. Cannot contain additional properties.
    - **`filepath`** *(string)*: Path of the log file. Empty filepath specifies output to console.
    - **`max_file_size`** *(integer)*: Maximum size of a file in bytes. Minimum: `1`.
    - **`max_files`** *(integer)*: Maximum number of files. Minimum: `1`.
    - **`output_log_level`** *(string)*: Minimum log level to write logs. Must be one of: `["trace", "debug", "info", "warn", "error", "critical"]`.
- **`client`** *(object)*: Configurations of clients. This is a mapping from configuration names to the configurations of clients. Cannot contain additional properties.
  - **`.+`** *(object)*: Configurations of clients. Cannot contain additional properties.
    - **`uris`** *(array)*: URIs of servers to connect to. URIs can be also added in ClientBuilder class.
      - **Items** *(string)*: A URI of a server to connect to.
    - **`call_timeout_sec`** *(number)*: Timeout of RPCs in seconds. Exclusive minimum: `0.0`.
    - **`message_parser`** *(object)*: Configurations of parsers of messages. Cannot contain additional properties.
      - **`read_buffer_size`** *(integer)*: Buffer size to read at once in bytes. Minimum: `1`.
    - **`executor`** *(object)*: Configurations of executors. Cannot contain additional properties.
      - **`num_transport_threads`** *(integer)*: Number of threads for transport. Minimum: `1`.
      - **`num_callback_threads`** *(integer)*: Number of threads for callbacks. Minimum: `1`.
    - **`reconnection`**: Configurations of reconnection to servers. Cannot contain additional properties.
      - **`initial_waiting_time_sec`** *(number)*: Initial waiting time. Exclusive minimum: `0.0`.
      - **`max_waiting_time_sec`** *(number)*: Maximum waiting time. Exclusive minimum: `0.0`.
      - **`max_jitter_waiting_time_sec`** *(number)*: Maximum jitter of waiting time. Exclusive minimum: `0.0`.
- **`server`** *(object)*: Cannot contain additional properties.
  - **`.+`** *(object)*
