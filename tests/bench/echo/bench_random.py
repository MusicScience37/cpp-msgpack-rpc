"""Execute benchmarks."""

import pathlib
import subprocess

import click
import pandas
import plotly.express

SAMPLES = 1000


@click.command()
@click.option("-b", "--build-dir", "build_dir_str", required=True)
@click.option("-s", "--samples", "samples", default=SAMPLES, type=int)
def run(build_dir_str: str, samples: int) -> None:
    build_dir_path = pathlib.Path(build_dir_str).absolute()
    bin_dir_path = build_dir_path / "bin"
    server_path = bin_dir_path / "bench_echo_server"
    client_path = bin_dir_path / "bench_echo_client_random"
    bench_output_path = build_dir_path / "bench" / "echo_random"
    csv_output_path = bench_output_path / "result.csv"

    bench_output_path.mkdir(parents=True, exist_ok=True)

    server_process = subprocess.Popen([str(server_path)])
    try:
        subprocess.run(
            [
                str(client_path),
                "--samples",
                str(samples),
                "--output",
                str(csv_output_path),
            ],
            check=True,
        )
    finally:
        server_process.terminate()
        server_process.wait(1)
        assert server_process.returncode == 0

    data_frame = pandas.read_csv(str(csv_output_path))

    protocol_key = "Protocol"
    data_size_key = "Data Size [byte]"
    duration_key = "Processing Time [sec]"

    figure = plotly.express.scatter(
        data_frame,
        x=data_size_key,
        y=duration_key,
        facet_col=protocol_key,
        log_x=True,
        log_y=True,
        width=1000,
    )
    figure.write_html(
        str(bench_output_path / "scatter.html"),
        include_plotlyjs="cdn",
    )
    figure.write_image(str(bench_output_path / "scatter.png"))


if __name__ == "__main__":
    run()
