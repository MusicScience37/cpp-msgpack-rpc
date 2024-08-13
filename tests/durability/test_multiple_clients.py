"""Test to request from multiple clients."""

import datetime
import pathlib
import shutil

from .process_executor import DEFAULT_PROCESS_WAIT_TIME, ProcessExecutor

THIS_DIR = pathlib.Path(__file__).absolute().parent


def _check_multiple_clients(
    *,
    bin_dir_path: pathlib.Path,
    current_log_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
    server_uri: str,
) -> None:
    config_file_path = THIS_DIR / "multiple_clients_config.toml"

    with ProcessExecutor(
        [
            str(bin_dir_path / "durability_serve_methods"),
            "--config",
            str(config_file_path),
            "--uri",
            server_uri,
            "--log",
            "server.log",
        ],
        cwd=str(current_log_dir_path),
        log_prefix="server",
    ) as server_process:
        with ProcessExecutor(
            [
                str(bin_dir_path / "durability_request_continuously"),
                "--config",
                str(config_file_path),
                "--uri",
                server_uri,
                "--log",
                "client1.log",
                "--seconds",
                str(int(test_duration.total_seconds())),
            ],
            cwd=str(current_log_dir_path),
            log_prefix="client1",
        ) as client1_process:
            with ProcessExecutor(
                [
                    str(bin_dir_path / "durability_request_continuously"),
                    "--config",
                    str(config_file_path),
                    "--uri",
                    server_uri,
                    "--log",
                    "client2.log",
                    "--seconds",
                    str(int(test_duration.total_seconds())),
                ],
                cwd=str(current_log_dir_path),
                log_prefix="client2",
            ) as client2_process:
                with ProcessExecutor(
                    [
                        str(bin_dir_path / "durability_request_continuously"),
                        "--config",
                        str(config_file_path),
                        "--uri",
                        server_uri,
                        "--log",
                        "client3.log",
                        "--seconds",
                        str(int(test_duration.total_seconds())),
                    ],
                    cwd=str(current_log_dir_path),
                    log_prefix="client3",
                ) as client3_process:
                    with ProcessExecutor(
                        [
                            str(bin_dir_path / "durability_request_continuously"),
                            "--config",
                            str(config_file_path),
                            "--uri",
                            server_uri,
                            "--log",
                            "client4.log",
                            "--seconds",
                            str(int(test_duration.total_seconds())),
                        ],
                        cwd=str(current_log_dir_path),
                        log_prefix="client4",
                    ) as client4_process:
                        client4_process.wait(
                            test_duration.total_seconds() * 2.0
                            + DEFAULT_PROCESS_WAIT_TIME
                        )
                    client3_process.wait()
                client2_process.wait()
            client1_process.wait()

    assert client1_process.returncode == 0
    assert server_process.returncode == 0


def test_multiple_clients_tcp_v4(
    bin_dir_path: pathlib.Path,
    log_parent_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
) -> None:
    """Test to request from multiple clients (TCPv4)."""
    current_log_dir_path = log_parent_dir_path / "test_multiple_clients_tcp_v4"
    if current_log_dir_path.exists():
        shutil.rmtree(str(current_log_dir_path))
    current_log_dir_path.mkdir(exist_ok=True, parents=True)

    server_uri = "tcp://127.0.0.1:12345"
    _check_multiple_clients(
        bin_dir_path=bin_dir_path,
        current_log_dir_path=current_log_dir_path,
        test_duration=test_duration,
        server_uri=server_uri,
    )


def test_multiple_clients_unix(
    bin_dir_path: pathlib.Path,
    log_parent_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
) -> None:
    """Test to request from multiple clients (Unix sockets)."""
    current_log_dir_path = log_parent_dir_path / "test_multiple_clients_unix"
    if current_log_dir_path.exists():
        shutil.rmtree(str(current_log_dir_path))
    current_log_dir_path.mkdir(exist_ok=True, parents=True)

    server_uri = "unix://durability_multiple_clients.sock"
    _check_multiple_clients(
        bin_dir_path=bin_dir_path,
        current_log_dir_path=current_log_dir_path,
        test_duration=test_duration,
        server_uri=server_uri,
    )
