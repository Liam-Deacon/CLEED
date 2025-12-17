# Agent / Contributor Guidance (docs)

## Canonical reference manual (do not remove)

`doc/CLEED_Manual.pdf` is the original CLEED manual written by Georg Held.
It is a key reference for understanding the original CLEED program suite:
inputs/outputs, algorithms, and legacy workflows that the codebase still
implements today.

- Keep `doc/CLEED_Manual.pdf` tracked in git.
- If you need to rename/move it, update any references and keep an equivalent
  canonical copy in the repository.

## Generated documentation output

`doc/_build/**` is Sphinx build output and should not be committed.
Historically a copy of the manual could appear at
`doc/_build/html/_downloads/CLEED_Manual.pdf`; treat that as generated output.
