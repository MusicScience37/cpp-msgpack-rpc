"""Configure pytest."""

import json
import pathlib
import typing

import fastjsonschema
import pytest

THIS_DIR = pathlib.Path(__file__).absolute().parent

SCHEMA_PATH = THIS_DIR.parent.parent / "schemas" / "config_schema_v1.json"


@pytest.fixture
def config_validator() -> typing.Callable[[str], bool]:
    with open(str(SCHEMA_PATH), mode="r", encoding="utf8") as file:
        data = json.load(file)
    validator = fastjsonschema.compile(data)

    def validate(config_data: str) -> bool:
        try:
            validator(config_data)
            return True
        except fastjsonschema.JsonSchemaException:
            return False

    return validate
