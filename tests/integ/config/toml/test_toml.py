"""Test of configuration files in TOML."""

import json
import pathlib
import subprocess
import typing

import approvaltests
import approvaltests.namer
import fastjsonschema
import pytest
import pytomlpp

THIS_DIR = pathlib.Path(__file__).absolute().parent

SCHEMA_PATH = THIS_DIR.parent.parent.parent.parent / "schemas" / "config_schema_v1.json"


@pytest.mark.parametrize(
    "name",
    [
        "no_config",
        "empty_configs",
        "full_configs",
    ],
)
def test_parse_correct_toml(name: str, toml_parser_path: pathlib.Path):
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


@pytest.fixture
def config_validator():
    with open(str(SCHEMA_PATH), mode="r", encoding="utf8") as file:
        data = json.load(file)
    return fastjsonschema.compile(data)


@pytest.mark.parametrize(
    "name",
    [
        "no_config",
        "empty_configs",
        "full_configs",
    ],
)
def test_validate_correct_yaml(
    name: str, config_validator: typing.Callable[[str], None]
):
    config_data = pytomlpp.load(str(THIS_DIR / f"{name}.toml"))
    config_validator(config_data)
