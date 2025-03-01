"""Configure pytest."""

import pathlib

import approvaltests
import pytest
from approvaltests.reporters import ReporterThatAutomaticallyApproves


def pytest_addoption(parser: pytest.Parser):
    """add option for pytest"""

    parser.addoption(
        "--build",
        action="store",
        required=True,
        help="path to build directory",
    )


@pytest.fixture(scope="session", autouse=True)
def configure_approvaltests():
    """Configure approvaltests library."""

    approvaltests.set_default_reporter(ReporterThatAutomaticallyApproves())


@pytest.fixture
def build_dir_path(request: pytest.FixtureRequest) -> pathlib.Path:
    return pathlib.Path(request.config.getoption("--build")).absolute()


@pytest.fixture
def writer_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "bin" / "integ_logging_write_log"


@pytest.fixture
def test_temp_dir_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "temp_test"
