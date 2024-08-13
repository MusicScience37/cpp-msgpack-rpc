"""Execute benchmarks."""

import pathlib
import subprocess

import click
import msgpack
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
                str(samples),
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

    with open(str(msgpack_output_path), mode="rb") as output_file:
        output_data = msgpack.unpack(output_file)
    protocol_list = []
    data_size_list = []
    duration_list = []
    for measurement in output_data["measurements"]:
        if measurement["measurer_name"] != "Processing Time":
            continue
        protocol = str(measurement["params"]["type"])
        data_size = str(measurement["params"]["size"])
        durations = measurement["durations"]["values"][0]
        num_samples = len(durations)
        protocol_list = protocol_list + [protocol] * num_samples
        data_size_list = data_size_list + [data_size] * num_samples
        duration_list = duration_list + durations
    protocol_key = "Protocol"
    data_size_key = "Data Size [byte]"
    duration_key = "Processing Time [sec]"
    data_frame = pandas.DataFrame(
        {
            protocol_key: protocol_list,
            data_size_key: data_size_list,
            duration_key: duration_list,
        }
    )

    figure = plotly.express.violin(
        data_frame,
        x=data_size_key,
        y=duration_key,
        color=protocol_key,
        log_y=True,
    )
    figure.write_image(str(bench_output_path / "violin.png"))
    figure.write_html(
        str(bench_output_path / "violin.html"),
        include_plotlyjs="cdn",
    )

    figure = plotly.express.box(
        data_frame,
        x=data_size_key,
        y=duration_key,
        color=protocol_key,
        log_y=True,
    )
    figure.write_image(str(bench_output_path / "box.png"))
    figure.write_html(
        str(bench_output_path / "box.html"),
        include_plotlyjs="cdn",
    )


if __name__ == "__main__":
    run()
