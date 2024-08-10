"""Test of client configurations in the schema of configurations."""

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
        "client": {
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
        "client": {
            "example": {
                "uris": value,
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        0.00001,
        1.0,
    ],
)
def test_correct_call_timeout_sec(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "client": {
            "example": {
                "call_timeout_sec": value,
            }
        }
    }
    config_checker.assert_valid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        None,
        "Any",
        0.0,
        -0.000001,
    ],
)
def test_invalid_call_timeout_sec(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "client": {
            "example": {
                "call_timeout_sec": value,
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
        "client": {
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
        "client": {
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
        "client": {
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
        "client": {
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
        "client": {
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
        "client": {
            "example": {
                "executor": {
                    "num_callback_threads": value,
                }
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        0.00001,
        1.0,
    ],
)
def test_correct_initial_waiting_time_sec(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "initial_waiting_time_sec": value,
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
        0.0,
        -0.000001,
    ],
)
def test_invalid_initial_waiting_time_sec(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "initial_waiting_time_sec": value,
                }
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        0.00001,
        1.0,
    ],
)
def test_correct_max_waiting_time_sec(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "max_waiting_time_sec": value,
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
        0.0,
        -0.000001,
    ],
)
def test_invalid_max_waiting_time_sec(value: typing.Any, config_checker: ConfigChecker):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "max_waiting_time_sec": value,
                }
            }
        }
    }
    config_checker.assert_invalid(config_data)


@pytest.mark.parametrize(
    "value",
    [
        0.00001,
        1.0,
    ],
)
def test_correct_max_jitter_waiting_time_sec(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "max_jitter_waiting_time_sec": value,
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
        0.0,
        -0.000001,
    ],
)
def test_invalid_max_jitter_waiting_time_sec(
    value: typing.Any, config_checker: ConfigChecker
):
    config_data = {
        "client": {
            "example": {
                "reconnection": {
                    "max_jitter_waiting_time_sec": value,
                }
            }
        }
    }
    config_checker.assert_invalid(config_data)
