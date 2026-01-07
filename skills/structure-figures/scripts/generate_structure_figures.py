#!/usr/bin/env python3
"""Generate schematic SVG figures for CLEED docs."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

SVG_UNIT_CELL = """<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"560\" height=\"360\" viewBox=\"0 0 560 360\">
  <rect width=\"560\" height=\"360\" fill=\"#ffffff\"/>
  <g stroke=\"#2f3b4a\" stroke-width=\"2\" fill=\"none\">
    <rect x=\"140\" y=\"70\" width=\"200\" height=\"200\"/>
    <rect x=\"220\" y=\"30\" width=\"200\" height=\"200\"/>
    <line x1=\"140\" y1=\"70\" x2=\"220\" y2=\"30\"/>
    <line x1=\"340\" y1=\"70\" x2=\"420\" y2=\"30\"/>
    <line x1=\"140\" y1=\"270\" x2=\"220\" y2=\"230\"/>
    <line x1=\"340\" y1=\"270\" x2=\"420\" y2=\"230\"/>
  </g>
  <g fill=\"#4f83ff\" stroke=\"#1b2a3a\" stroke-width=\"1\">
    <circle cx=\"140\" cy=\"70\" r=\"6\"/>
    <circle cx=\"340\" cy=\"70\" r=\"6\"/>
    <circle cx=\"140\" cy=\"270\" r=\"6\"/>
    <circle cx=\"340\" cy=\"270\" r=\"6\"/>
    <circle cx=\"220\" cy=\"30\" r=\"6\"/>
    <circle cx=\"420\" cy=\"30\" r=\"6\"/>
    <circle cx=\"220\" cy=\"230\" r=\"6\"/>
    <circle cx=\"420\" cy=\"230\" r=\"6\"/>
    <circle cx=\"280\" cy=\"150\" r=\"6\"/>
  </g>
  <g stroke=\"#2f3b4a\" stroke-width=\"2\">
    <line x1=\"140\" y1=\"300\" x2=\"240\" y2=\"300\"/>
    <line x1=\"140\" y1=\"300\" x2=\"140\" y2=\"240\"/>
    <line x1=\"140\" y1=\"300\" x2=\"190\" y2=\"260\"/>
  </g>
  <g font-family=\"Arial, sans-serif\" font-size=\"14\" fill=\"#1b2a3a\">
    <text x=\"248\" y=\"305\">a1</text>
    <text x=\"120\" y=\"235\">a2</text>
    <text x=\"190\" y=\"250\">a3</text>
    <text x=\"40\" y=\"40\">Schematic fcc unit cell (Ni/Cu example)</text>
  </g>
</svg>
"""

SVG_SURFACE_SLAB = """<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"560\" height=\"360\" viewBox=\"0 0 560 360\">
  <rect width=\"560\" height=\"360\" fill=\"#ffffff\"/>
  <g font-family=\"Arial, sans-serif\" font-size=\"14\" fill=\"#1b2a3a\">
    <text x=\"40\" y=\"40\">Schematic Ni(111) slab (side view)</text>
  </g>
  <g fill=\"#7a8da6\" stroke=\"#1b2a3a\" stroke-width=\"1\">
    <circle cx=\"140\" cy=\"110\" r=\"10\"/>
    <circle cx=\"200\" cy=\"110\" r=\"10\"/>
    <circle cx=\"260\" cy=\"110\" r=\"10\"/>
    <circle cx=\"170\" cy=\"150\" r=\"10\"/>
    <circle cx=\"230\" cy=\"150\" r=\"10\"/>
    <circle cx=\"290\" cy=\"150\" r=\"10\"/>
    <circle cx=\"140\" cy=\"190\" r=\"10\"/>
    <circle cx=\"200\" cy=\"190\" r=\"10\"/>
    <circle cx=\"260\" cy=\"190\" r=\"10\"/>
    <circle cx=\"170\" cy=\"230\" r=\"10\"/>
    <circle cx=\"230\" cy=\"230\" r=\"10\"/>
    <circle cx=\"290\" cy=\"230\" r=\"10\"/>
  </g>
  <g stroke=\"#1b2a3a\" stroke-width=\"2\">
    <line x1=\"360\" y1=\"240\" x2=\"360\" y2=\"120\"/>
    <polygon points=\"360,110 354,122 366,122\" fill=\"#1b2a3a\"/>
  </g>
  <g font-family=\"Arial, sans-serif\" font-size=\"13\" fill=\"#1b2a3a\">
    <text x=\"372\" y=\"150\">surface normal</text>
    <text x=\"372\" y=\"168\">(111)</text>
  </g>
</svg>
"""

SVG_OVERLAYER = """<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"560\" height=\"360\" viewBox=\"0 0 560 360\">
  <rect width=\"560\" height=\"360\" fill=\"#ffffff\"/>
  <g font-family=\"Arial, sans-serif\" font-size=\"14\" fill=\"#1b2a3a\">
    <text x=\"40\" y=\"40\">Schematic 2x2 O overlayer on Ni(111) (top view)</text>
  </g>
  <g fill=\"#7a8da6\" stroke=\"#1b2a3a\" stroke-width=\"1\">
    <circle cx=\"160\" cy=\"140\" r=\"10\"/>
    <circle cx=\"240\" cy=\"140\" r=\"10\"/>
    <circle cx=\"320\" cy=\"140\" r=\"10\"/>
    <circle cx=\"200\" cy=\"200\" r=\"10\"/>
    <circle cx=\"280\" cy=\"200\" r=\"10\"/>
    <circle cx=\"360\" cy=\"200\" r=\"10\"/>
    <circle cx=\"160\" cy=\"260\" r=\"10\"/>
    <circle cx=\"240\" cy=\"260\" r=\"10\"/>
    <circle cx=\"320\" cy=\"260\" r=\"10\"/>
  </g>
  <g fill=\"#e34b4b\" stroke=\"#1b2a3a\" stroke-width=\"1\">
    <circle cx=\"200\" cy=\"140\" r=\"8\"/>
    <circle cx=\"280\" cy=\"260\" r=\"8\"/>
  </g>
  <g stroke=\"#1b2a3a\" stroke-width=\"2\" fill=\"none\">
    <rect x=\"150\" y=\"130\" width=\"200\" height=\"200\" stroke-dasharray=\"6 4\"/>
  </g>
  <g font-family=\"Arial, sans-serif\" font-size=\"13\" fill=\"#1b2a3a\">
    <text x=\"365\" y=\"250\">2x2 cell</text>
    <text x=\"370\" y=\"140\">Ni</text>
    <text x=\"370\" y=\"160\">O adatom</text>
  </g>
  <g stroke=\"#1b2a3a\" stroke-width=\"1.5\">
    <line x1=\"355\" y1=\"136\" x2=\"340\" y2=\"136\"/>
    <line x1=\"355\" y1=\"156\" x2=\"320\" y2=\"156\"/>
  </g>
</svg>
"""

FIGURES = {
    "ni111_unit_cell.svg": SVG_UNIT_CELL,
    "ni111_surface_slab.svg": SVG_SURFACE_SLAB,
    "ni111_2x2o_overlayer.svg": SVG_OVERLAYER,
}


def _check_outputs(out_dir: Path) -> list[Path]:
    missing = []
    for name, content in FIGURES.items():
        path = out_dir / name
        if not path.exists():
            missing.append(path)
            continue
        if path.read_text(encoding="utf-8") != content:
            missing.append(path)
    return missing


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--out-dir",
        default="doc/_static/figures",
        help="Output directory for generated SVGs",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="Check whether outputs match without writing",
    )
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    if args.check:
        missing = _check_outputs(out_dir)
        if missing:
            for path in missing:
                print(f"out-of-date: {path}", file=sys.stderr)
            return 1
        return 0

    out_dir.mkdir(parents=True, exist_ok=True)
    for name, content in FIGURES.items():
        try:
            (out_dir / name).write_text(content, encoding="utf-8")
        except OSError as exc:
            print(f"Error writing {out_dir / name}: {exc}", file=sys.stderr)
            return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
