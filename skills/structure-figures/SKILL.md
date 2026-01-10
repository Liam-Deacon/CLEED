---
name: structure-figures
description: Generate or update schematic structure figures (SVGs) used in CLEED docs and example inputs.
metadata:
  short-description: Generate structure SVGs
---

# Structure Figures

Use this skill when adding or updating schematic structure figures for the CLEED
examples and docs (e.g., Ni111 unit cell, Ni(111) slab, 2x2 O overlayer).

## Quick start

Generate or refresh the SVGs in `doc/_static/figures`:

```bash
python3 skills/structure-figures/scripts/generate_structure_figures.py \
  --out-dir doc/_static/figures
```

## Notes

- The script is the source of truth for these assets:
  - `doc/_static/figures/ni111_unit_cell.svg`
  - `doc/_static/figures/ni111_surface_slab.svg`
  - `doc/_static/figures/ni111_2x2o_overlayer.svg`
- The figures are referenced from `doc/file_formats/bul.rst` and
  `doc/file_formats/inp.rst`; update captions or cross-references there if the
  examples change.
- If PNGs are required for a downstream target, convert from SVG using a local
  tool (e.g., `rsvg-convert` or `inkscape`) and commit only the resulting static
  asset files.
