"""Test of server configurations in the schema of configurations."""

import typing

import pytest

from .config_checker import ConfigChecker


@pytest.mark.parametrize(
    "value",
    [
        [],
        ["tcp://localhost:12345"],
        ["tcp://localhost:12345", "tcp://localhost:1234"],
    ],
)
def test_correct_uris(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "uris": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        "tcp://localhost:12345",
        123,
        [123],
    ],
)
def test_invalid_uris(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "uris": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        1,
        2,
        1000,
    ],
)
def test_correct_read_buffer_size(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "message_parser": {
                    "read_buffer_size": value,
                }
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
def test_invalid_read_buffer_size(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "message_parser": {
                    "read_buffer_size": value,
                }
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
def test_correct_num_transport_threads(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "server": {
            "example": {
                "executor": {
                    "num_transport_threads": value,
                }
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
def test_invalid_num_transport_threads(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "server": {
            "example": {
                "executor": {
                    "num_transport_threads": value,
                }
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
def test_correct_num_callback_threads(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "executor": {
                    "num_callback_threads": value,
                }
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
def test_invalid_num_callback_threads(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "server": {
            "example": {
                "executor": {
                    "num_callback_threads": value,
                }
            }
        }
    }
    config_checker.assert_invalid(config_data)
