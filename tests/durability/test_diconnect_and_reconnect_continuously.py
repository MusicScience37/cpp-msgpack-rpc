"""Test to disconnect and reconnect clients continuously."""

import datetime
import pathlib
import shutil

from .process_executor import DEFAULT_PROCESS_WAIT_TIME, ProcessExecutor

THIS_DIR = pathlib.Path(__file__).absolute().parent


CLIENT_EXECUTION_TIME = datetime.timedelta(seconds=1)


def _check_disconnect_and_reconnect_continuously(
    *,
    bin_dir_path: pathlib.Path,
    current_log_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
    server_uri: str,
) -> None:
    config_file_path = THIS_DIR / "minimal_config.toml"

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
        finish_time = datetime.datetime.now() + test_duration
        while datetime.datetime.now() < finish_time:
            with ProcessExecutor(
                [
                    str(bin_dir_path / "durability_request_continuously"),
                    "--config",
                    str(config_file_path),
                    "--uri",
                    server_uri,
                    "--log",
                    "client.log",
                    "--seconds",
                    str(int(CLIENT_EXECUTION_TIME.total_seconds())),
                ],
                cwd=str(current_log_dir_path),
                log_prefix="client",
                write_plots=False,
            ) as client_process:
                client_process.wait(
                    CLIENT_EXECUTION_TIME.total_seconds() + DEFAULT_PROCESS_WAIT_TIME
                )

            assert client_process.returncode == 0

    assert server_process.returncode == 0


def test_disconnect_and_reconnect_continuously_tcp_v4(
    bin_dir_path: pathlib.Path,
    log_parent_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
) -> None:
    """Test to disconnect and reconnect clients continuously (TCPv4)."""
    current_log_dir_path = (
        log_parent_dir_path / "test_disconnect_and_reconnect_continuously_tcp_v4"
    )
    if current_log_dir_path.exists():
        shutil.rmtree(str(current_log_dir_path))
    current_log_dir_path.mkdir(exist_ok=True, parents=True)

    server_uri = "tcp://127.0.0.1:12345"

    _check_disconnect_and_reconnect_continuously(
        bin_dir_path=bin_dir_path,
        current_log_dir_path=current_log_dir_path,
        test_duration=test_duration,
        server_uri=server_uri,
    )


def test_disconnect_and_reconnect_continuously_unix(
    bin_dir_path: pathlib.Path,
    log_parent_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
) -> None:
    """Test to disconnect and reconnect clients continuously (Unix socket)."""
    current_log_dir_path = (
        log_parent_dir_path / "test_disconnect_and_reconnect_continuously_unix"
    )
    if current_log_dir_path.exists():
        shutil.rmtree(str(current_log_dir_path))
    current_log_dir_path.mkdir(exist_ok=True, parents=True)

    server_uri = "unix://durability_request_continuous.sock"

    _check_disconnect_and_reconnect_continuously(
        bin_dir_path=bin_dir_path,
        current_log_dir_path=current_log_dir_path,
        test_duration=test_duration,
        server_uri=server_uri,
    )
