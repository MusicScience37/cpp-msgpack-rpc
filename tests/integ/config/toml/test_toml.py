"""Test of configuration files in TOML."""

import pathlib
import subprocess

import approvaltests
import approvaltests.namer
import pytest


THIS_DIR = pathlib.Path(__file__).absolute().parent


@pytest.mark.parametrize(
    "name",
    [
        "empty",
        "one_empty_config",
        "one_full_config",
    ],
)
def test_correct_toml(name: str, toml_parser_path: pathlib.Path):
    result = subprocess.run(
        [
            str(toml_parser_path),
            str(THIS_DIR / f"{name}.toml"),
        ],
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        encoding="utf8",
    )
    assert result.returncode == 0
    approvaltests.approvals.verify(
        result.stdout,
        options=approvaltests.namer.NamerFactory.with_parameters(name),
    )
