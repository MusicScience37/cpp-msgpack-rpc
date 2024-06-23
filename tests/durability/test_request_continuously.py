"""Test to request continuously."""

import datetime
import pathlib
import shutil

from .process_executor import ProcessExecutor


def test_request_continuously_tcp_v4(
    bin_dir_path: pathlib.Path,
    log_parent_dir_path: pathlib.Path,
    test_duration: datetime.timedelta,
) -> None:
    """Test to request continuously (TCPv4)."""
    current_log_dir_path = log_parent_dir_path / "test_request_continuously"
    if current_log_dir_path.exists():
        shutil.rmtree(str(current_log_dir_path))
    current_log_dir_path.mkdir(exist_ok=True, parents=True)

    server_uri = "tcp://127.0.0.1:12345"

    with ProcessExecutor(
        [
            str(bin_dir_path / "durability_serve_methods"),
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
                "--uri",
                server_uri,
                "--log",
                "client.log",
                "--seconds",
                str(int(test_duration.total_seconds())),
            ],
            cwd=str(current_log_dir_path),
            log_prefix="client",
        ) as client_process:
            client_process.wait(test_duration.total_seconds() * 2.0)

    assert client_process.returncode == 0
    assert server_process.returncode == 0
