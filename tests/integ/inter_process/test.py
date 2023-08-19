"""Execute the test."""

import subprocess
import pathlib
import click


@click.command()
@click.option("-b", "--build-dir", "build_dir_str", required=True)
def run(build_dir_str: str):
    build_dir_path = pathlib.Path(build_dir_str).absolute()
    bin_dir_path = build_dir_path / "bin"
    server_path = bin_dir_path / "integ_inter_process_server"
    client_path = bin_dir_path / "integ_inter_process_client"

    server_process = subprocess.Popen([str(server_path)])
    try:
        subprocess.run([client_path], check=True)
    finally:
        server_process.terminate()
        server_process.wait(1)
        assert server_process.returncode == 0

    print("Test succeeded!")


if __name__ == "__main__":
    run()
