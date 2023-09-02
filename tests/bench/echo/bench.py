"""Execute benchmarks."""

import subprocess
import pathlib
import click


SAMPLES = 1000


@click.command()
@click.option("-b", "--build-dir", "build_dir_str", required=True)
def run(build_dir_str: str):
    build_dir_path = pathlib.Path(build_dir_str).absolute()
    bin_dir_path = build_dir_path / "bin"
    server_path = bin_dir_path / "bench_echo_server"
    client_path = bin_dir_path / "bench_echo_client"
    bench_output_path = build_dir_path / "bench" / "echo"
    json_output_path = bench_output_path / "result.json"
    msgpack_output_path = bench_output_path / "result.msgpack"

    server_process = subprocess.Popen([str(server_path)])
    try:
        subprocess.run(
            [
                str(client_path),
                "--mean_samples",
                "1",
                "--samples",
                str(SAMPLES),
                "--json",
                str(json_output_path),
                "--msgpack",
                str(msgpack_output_path),
                "--plot",
                str(bench_output_path),
            ],
            check=True,
        )
    finally:
        server_process.terminate()
        server_process.wait(1)
        assert server_process.returncode == 0


if __name__ == "__main__":
    run()
