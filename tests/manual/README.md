# Manual testing

This directory contains tests which require manual execution.

## Test of reconnection with exponential backoff

Execute `manual_test_reconnection_client` to check waiting time in exponential backoff.

```console
$ ./build/Debug/bin/manual_test_reconnection_client
[2023-08-15 14:26:38.546008] [warning] (Connector(tcp)) Failed to connect to tcp://localhost:12345: Connection refused (ip_connector.h:168, on_connect)
[2023-08-15 14:26:38.546340] [warning] Failed to connect to all URIs, so retry after 0.233 seconds. (reconnection_timer.h:80, async_wait)
[2023-08-15 14:26:38.780016] [warning] (Connector(tcp)) Failed to connect to tcp://localhost:12345: Connection refused (ip_connector.h:168, on_connect)
[2023-08-15 14:26:38.780132] [warning] Failed to connect to all URIs, so retry after 0.305 seconds. (reconnection_timer.h:80, async_wait)
[2023-08-15 14:26:39.085385] [warning] (Connector(tcp)) Failed to connect to tcp://localhost:12345: Connection refused (ip_connector.h:168, on_connect)
[2023-08-15 14:26:39.085483] [warning] Failed to connect to all URIs, so retry after 0.575 seconds. (reconnection_timer.h:80, async_wait)
[2023-08-15 14:26:39.661347] [warning] (Connector(tcp)) Failed to connect to tcp://localhost:12345: Connection refused (ip_connector.h:168, on_connect)
[2023-08-15 14:26:39.661485] [warning] Failed to connect to all URIs, so retry after 1.003 seconds. (reconnection_timer.h:80, async_wait)
```
