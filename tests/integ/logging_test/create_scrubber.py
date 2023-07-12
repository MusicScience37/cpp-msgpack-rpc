"""Scrubber to use in approvaltests.
"""

import pathlib
import re
import typing

import approvaltests.scrubbers


def _create_timestamp_scrubber() -> typing.Callable[[str], str]:
    return approvaltests.scrubbers.create_regex_scrubber(
        r"\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d\.\d\d\d\d\d\d",
        "<time-stamp>",
    )


def _scrub_file_log_location(input: str) -> str:
    def replace_path(match: re.Match[str]) -> str:
        filename = pathlib.Path(match.group(1)).name
        line = match.group(2)

        # Function name can differ between different compilers.
        return f"({filename}:{line}, <function-name>)"

    return re.sub(r"\((.*):(\d*), (.*)\)", replace_path, input)


def create_scrubber() -> typing.Callable[[str], str]:
    return approvaltests.scrubbers.combine_scrubbers(
        _create_timestamp_scrubber(),
        _scrub_file_log_location,
    )
