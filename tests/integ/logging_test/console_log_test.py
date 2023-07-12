"""Test of console logs.
"""

import pathlib
import subprocess
import typing

import approvaltests
from .create_scrubber import create_scrubber


def _verify_command_result(
    writer_path: pathlib.Path,
    args: typing.List[str],
    test_temp_dir_path: pathlib.Path,
) -> None:
    result = subprocess.run(
        args=[str(writer_path)] + args,
        cwd=str(test_temp_dir_path),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        encoding="utf-8",
    )

    approvaltests.verify(
        f"""exit code: {result.returncode}
stdout:
{result.stdout}
stderr:
{result.stderr}
""",
        options=approvaltests.Options().with_scrubber(create_scrubber()),
    )


def test_info_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
):
    """Test with info log level."""

    _verify_command_result(
        writer_path=writer_path,
        args=[],
        test_temp_dir_path=test_temp_dir_path,
    )


def test_trace_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
):
    """Test with trace log level."""

    _verify_command_result(
        writer_path=writer_path,
        args=["-v"],
        test_temp_dir_path=test_temp_dir_path,
    )


def test_quiet(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
):
    """Test of null log sink."""

    _verify_command_result(
        writer_path=writer_path,
        args=["-q"],
        test_temp_dir_path=test_temp_dir_path,
    )
