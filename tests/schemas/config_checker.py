"""Implementation of ConfigChecker class."""

import json
import pathlib

import fastjsonschema

THIS_DIR = pathlib.Path(__file__).absolute().parent

SCHEMA_PATH = THIS_DIR.parent.parent / "schemas" / "config_schema_v1.json"


class ConfigChecker:
    """Class to check configurations using the schema."""

    def __init__(self) -> None:
        with open(str(SCHEMA_PATH), mode="r", encoding="utf8") as file:
            data = json.load(file)
        self._validator = fastjsonschema.compile(data)

    def assert_valid(self, config_data: str) -> None:
        """Assert that the given configuration is valid.

        Args:
            config_data (str): Data of the configuration.
        """
        self._validator(config_data)

    def assert_invalid(self, config_data: str) -> None:
        """Assert that the given configuration is invalid.

        Args:
            config_data (str): Data of the configuration.
        """
        try:
            self._validator(config_data)
            raise RuntimeError("Configuration is valid.")
        except fastjsonschema.JsonSchemaException:
            pass
