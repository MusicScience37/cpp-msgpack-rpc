#!/usr/bin/env python3
"""Check encoding of files."""

from sys import argv


def check_encoding(file_path: str) -> str:
    """Check encoding of a file.

    Args:
        file_path (str): File path.
    """
    with open(file_path, mode="r", encoding="utf8") as file:
        return file.read()


if __name__ == "__main__":
    for arg in argv:
        check_encoding(arg)
