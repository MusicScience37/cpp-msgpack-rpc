"""Test of logging configurations in the schema of configurations."""

import typing

import pytest

from .config_checker import ConfigChecker


@pytest.mark.parametrize(
    "value",
    [
        "",
        "file/path.log",
    ],
)
def test_correct_filepath(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "filepath": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        123,
    ],
)
def test_invalid_filepath(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "filepath": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        1,
        2,
        10,
    ],
)
def test_correct_max_file_size(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "max_file_size": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


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
def test_invalid_max_file_size(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "max_file_size": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        1,
        2,
        10,
    ],
)
def test_correct_max_files(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "max_files": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


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
def test_invalid_max_files(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "max_files": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


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
def test_correct_output_log_level(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "output_log_level": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        123,
        "any",
    ],
)
def test_invalid_output_log_level(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "output_log_level": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


def test_invalid_property(config_checker: ConfigChecker):
    config_data = {
        "logging": {
            "example": {
                "any": "value",
            }
        }
    }
    config_checker.assert_invalid(config_data)
