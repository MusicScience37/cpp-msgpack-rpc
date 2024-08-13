"""Test of RPCs between processes."""

import pathlib
import subprocess


def _check_inter_process_rpc(build_dir_path: pathlib.Path, uri: str) -> None:
    bin_dir_path = build_dir_path / "bin"
    server_path = bin_dir_path / "integ_inter_process_server"
    client_path = bin_dir_path / "integ_inter_process_client"

    server_process = subprocess.Popen([str(server_path), uri])
    try:
        subprocess.run([str(client_path), uri], check=True)
    finally:
        server_process.terminate()
        server_process.wait(1)
        assert server_process.returncode == 0


def test_inter_process_rpc_tcp(build_dir_path: pathlib.Path) -> None:
    """Test of RPCs between processes using TCP."""
    uri = "tcp://localhost:4931"
    _check_inter_process_rpc(build_dir_path=build_dir_path, uri=uri)


def test_inter_process_rpc_tcp(build_dir_path: pathlib.Path) -> None:
    """Test of RPCs between processes using TCP."""
    uri = "unix://integ_inter_process.sock"
    _check_inter_process_rpc(build_dir_path=build_dir_path, uri=uri)
