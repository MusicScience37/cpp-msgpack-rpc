"""Test of logging configurations in the schema of configurations."""

import json
import typing

import fastjsonschema
import pytest


@pytest.mark.parametrize(
    "value",
    [
        "",
        "file/path.log",
    ],
)
def test_validate_correct_filepath(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "filepath": value,
            }
        }
    }
    assert config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        123,
    ],
)
def test_validate_invalid_filepath(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "filepath": value,
            }
        }
    }
    assert not config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        1,
        2,
        10,
    ],
)
def test_validate_correct_max_file_size(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "max_file_size": value,
            }
        }
    }
    assert config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        "Any",
        0,
        -1,
        -10,
    ],
)
def test_validate_invalid_max_file_size(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "max_file_size": value,
            }
        }
    }
    assert not config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        1,
        2,
        10,
    ],
)
def test_validate_correct_max_files(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "max_files": value,
            }
        }
    }
    assert config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        "Any",
        0,
        -1,
        -10,
    ],
)
def test_validate_invalid_max_files(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "max_files": value,
            }
        }
    }
    assert not config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        "trace",
        "debug",
        "info",
        "warn",
        "error",
        "critical",
    ],
)
def test_validate_correct_output_log_level(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "output_log_level": value,
            }
        }
    }
    assert config_validator(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        123,
        "any",
    ],
)
def test_validate_invalid_output_log_level(
    value: typing.Any, config_validator: typing.Callable[[str], bool]
):
    config_data = {
        "logging": {
            "example": {
                "output_log_level": value,
            }
        }
    }
    assert not config_validator(config_data)
