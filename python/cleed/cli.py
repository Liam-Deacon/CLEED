"""CLI wrapper for invoking CLEED executables from a Python install."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from typing import List

from . import __version__

TOOLS = {
    "cleed_nsym": "Run the non-symmetry CLEED solver",
    "cleed_sym": "Run the symmetry CLEED solver",
    "csearch": "Run the structure search driver",
    "crfac": "Compute R-factors",
}


def resolve_tool(name: str) -> str | None:
    return shutil.which(name)


def run_tool(binary: str, args: List[str]) -> int:
    try:
        result = subprocess.run([binary] + args, check=False)
    except FileNotFoundError:
        return 127
    return result.returncode


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Run CLEED executables from a Python install.",
    )
    parser.add_argument(
        "--list-tools",
        action="store_true",
        help="List supported tools and exit",
    )
    parser.add_argument(
        "--version",
        action="store_true",
        help="Print the cleed Python package version",
    )
    parser.add_argument(
        "tool",
        nargs="?",
        help="Executable to run (e.g. cleed_nsym, csearch)",
    )
    parser.add_argument(
        "args",
        nargs=argparse.REMAINDER,
        help="Arguments passed through to the underlying executable",
    )
    return parser


def main(argv: List[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if args.version:
        print(__version__)
        return 0

    if args.list_tools:
        for name, description in TOOLS.items():
            print(f"{name}\t{description}")
        return 0

    if not args.tool:
        parser.print_help()
        return 2

    if args.tool not in TOOLS:
        print(f"Unknown tool: {args.tool}", file=sys.stderr)
        print("Use --list-tools to see available options.", file=sys.stderr)
        return 2

    binary = resolve_tool(args.tool)
    if not binary:
        print(
            f"Executable not found on PATH: {args.tool}",
            file=sys.stderr,
        )
        return 127

    tool_args = args.args
    if tool_args and tool_args[0] == "--":
        tool_args = tool_args[1:]

    return run_tool(binary, tool_args)


if __name__ == "__main__":
    raise SystemExit(main())
