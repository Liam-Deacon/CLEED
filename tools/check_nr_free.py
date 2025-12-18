#!/usr/bin/env python3
"""
Guard against re-introducing non-redistributable legacy code markers.

This is intentionally conservative and only scans the current source tree:
  - src/
  - src/include/
"""

from __future__ import annotations

import os
import sys
from pathlib import Path


BANNED_SUBSTRINGS = (
    "Numerical Recipes",
    "Num. Rec.",
    "Num. Rec",
)


def iter_source_files(root: Path) -> list[Path]:
    paths: list[Path] = []
    for base in (root / "src", root / "src" / "include"):
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if path.is_file():
                paths.append(path)
    return paths


def main() -> int:
    repo_root = Path(os.getcwd()).resolve()

    failures: list[str] = []

    for path in iter_source_files(repo_root):
        rel = path.relative_to(repo_root).as_posix()

        # Disallow legacy NR-prefixed file naming in the source tree.
        if path.name.startswith("nrr"):
            failures.append(f"{rel}: forbidden legacy filename prefix 'nrr'")
            continue

        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError as exc:
            failures.append(f"{rel}: failed to read ({exc})")
            continue

        for needle in BANNED_SUBSTRINGS:
            if needle in text:
                failures.append(f"{rel}: contains banned marker {needle!r}")

    if failures:
        sys.stderr.write("NR guard failed:\n")
        for item in failures:
            sys.stderr.write(f"  - {item}\n")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

