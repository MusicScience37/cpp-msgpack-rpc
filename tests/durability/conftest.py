"""Configure pytest."""

import datetime
import pathlib

import pytest


def pytest_addoption(parser: pytest.Parser):
    """add option for pytest"""

    parser.addoption(
        "--build",
        action="store",
        required=True,
        help="path to build directory",
    )
    parser.addoption(
        "--seconds",
        action="store",
        type=int,
        default=3,
        help="test seconds",
    )


@pytest.fixture
def build_dir_path(request: pytest.FixtureRequest) -> pathlib.Path:
    return pathlib.Path(request.config.getoption("--build")).absolute()


@pytest.fixture
def test_duration(request: pytest.FixtureRequest) -> datetime.timedelta:
    return datetime.timedelta(seconds=int(request.config.getoption("--seconds")))


@pytest.fixture
def bin_dir_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "bin"


@pytest.fixture
def test_temp_dir_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "temp_test"


@pytest.fixture
def log_parent_dir_path(test_temp_dir_path: pathlib.Path) -> pathlib.Path:
    return test_temp_dir_path / "durability"
