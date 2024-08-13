"""Test of logs using rotating file log sink.
"""

import pathlib
import subprocess
import typing

import approvaltests

from .create_scrubber import create_scrubber

THIS_DIR = pathlib.Path(__file__).absolute().parent
CONFIG_PATH = THIS_DIR / "rotating_file_log_configs.toml"


def _verify_command_result(
    writer_path: pathlib.Path,
    args: typing.List[str],
    test_temp_dir_path: pathlib.Path,
    log_filename: str,
) -> None:
    result = subprocess.run(
        args=[str(writer_path)] + args,
        cwd=str(test_temp_dir_path),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        encoding="utf-8",
    )

    with open(
        str(test_temp_dir_path / log_filename), mode="r", encoding="utf-8"
    ) as file:
        log_file_contents = file.read()

    approvaltests.verify(
        f"""exit code: {result.returncode}
stdout:
{result.stdout}
stderr:
{result.stderr}
{log_filename}:
{log_file_contents}
""",
        options=approvaltests.Options().with_scrubber(create_scrubber()),
    )


def test_info_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
):
    """Test with info log level."""

    log_filename = "msgpack_rpc_test_integ_logging.log"
    _verify_command_result(
        writer_path=writer_path,
        args=[
            "--config-file",
            str(CONFIG_PATH),
            "--config-name",
            "info",
        ],
        test_temp_dir_path=test_temp_dir_path,
        log_filename=log_filename,
    )


def test_trace_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
):
    """Test with trace log level."""

    log_filename = "msgpack_rpc_test_integ_logging.log"
    _verify_command_result(
        writer_path=writer_path,
        args=[
            "--config-file",
            str(CONFIG_PATH),
            "--config-name",
            "trace",
        ],
        test_temp_dir_path=test_temp_dir_path,
        log_filename=log_filename,
    )
