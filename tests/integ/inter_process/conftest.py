"""Configure pytest."""

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


@pytest.fixture
def build_dir_path(request: pytest.FixtureRequest) -> pathlib.Path:
    return pathlib.Path(request.config.getoption("--build")).absolute()
