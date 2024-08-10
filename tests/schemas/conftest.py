"""Configure pytest."""

import pathlib

import pytest

from .config_checker import ConfigChecker

THIS_DIR = pathlib.Path(__file__).absolute().parent


@pytest.fixture
def config_checker() -> ConfigChecker:
    return ConfigChecker()
