# CLEED Codebase Review

This document captures the initial findings from working in the `master` tree. It is intended to serve as both a quick reference for the current blockers and a guideline for the upcoming modernization/package/QA work outlined in the roadmap.

## 1. Build and toolchain

| Symptom | Details | Severity | Recommended immediate action |
| --- | --- | --- | --- |
| `cmake` fails on macOS | Qt4 components (`QtSvg`, `QtGui`, `QtCore`, `QtNetwork`) and `qmake` are missing; configuration aborts with `Found unsuitable Qt version ""` | Critical | Keep `WITH_QT=ON` guarded until Qt6 support exists or update CMake to look for Qt6/QT6 components; install Qt4 or gate GUI build; document in README |
| Pre-commit hook missing | Repository lacks a `.pre-commit-config.yaml`, which makes the `python3 -m pre_commit run` step impossible | Medium | Add a portable `.pre-commit-config.yaml` (formatting + lint) and document hook invocation in AGENTS/README |
| Codacy/C test warnings | `lattice.c` and related tests exceed Codacy complexity/length thresholds plus `sscanf` without width specifiers | Medium | Break up helpers (already underway) and add bounds checks/cast improvements to satisfy static analysis |

### Observed output snippet
```
-- Found unsuitable Qt version "" from NOTFOUND
CMake Error at /opt/homebrew/share/cmake/Modules/FindQt4.cmake:1469 (message):
  Found unsuitable Qt version "" from NOTFOUND, this code requires Qt 4.x
```

## 2. Modernization opportunities

1. **Qt6 readiness:** the GUI targets still request Qt4. There is no abstraction for `QT6` modules (no `target_link_libraries` for modern components). Qt4-specific code (ifdefs, `#include <QtGui>`, etc.) needs to be rewritten or isolated.
2. **Image codecs:** `libtiff`, `libjpeg` usage is ad-hoc; rebuilding with current wrappers (e.g., libjpeg-turbo, libtiff modern API) will reduce build pain and align with current dependencies.
3. **Numerical Recipes (C) usage:** parts of `src` still depend on NR algorithms with restrictive licensing. Replace NR with BSD/Apache equivalents or implement sanitized versions; add regression tests to confirm behavior remains unchanged.

| Component | Concern | Suggested alternative |
| --- | --- | --- |
| Qt `FindQt4.cmake` usage | Unsupported Qt version on modern systems | Move to Qt6's `find_package(Qt6 COMPONENTS Core Gui Widgets Network Svg REQUIRED)` with `cmake_minimum_required(3.26)` or similar |
| `src/latt/latt.c` loops | Complexity hitting Codacy limits | Delegate to helper functions, reduce duplication, convert `n_bas` to `size_t`, propagate errors instead of `exit(1)` |

## 3. Testing and regression protection

- There are no dedicated unit tests guarding against Numerical Recipes behavior or `n_bas` conversion issues.
- Recommendation: create deterministic tests (e.g., `tests/test_lattice_*`) around boundary conditions, and assert convergence counts.
- The review now has `tests/test_search_amoeba` with a long `main()` function; refactor into helper states with a clear assertion block.

```
[caller] -> [lattice_setup] -> [amoeba search] -> [state assertions]
``` (target to cover >90% of crucial branches)

## 4. Packaging, automation, and releases

Goals from the roadmap:
- Windows installer
- Homebrew formula for macOS
- .deb and .rpm for Linux
- Docker image published through GitHub Actions

Ascii release pipeline:

```
[Source   ]
    |
    v
[CI Build ] -> [Unit Tests]
    |
    v
[Packaging (Windows/MSI, Homebrew, deb/rpm, Docker)]
    |
    v
[GitHub Releases Assets (installer, tarballs, Docker image)]
```

Each packaging target will need cross-platform toolchains, but the first step is to make the code build with modern dependencies and ensure deterministic output.

## 5. Documentation and Sphinx

- The manual exists in legacy format; convert (via Pandoc) to Markdown and/or reStructuredText, then integrate with Sphinx.
- Sphinx docs should be automated in CI, with build artifacts published to ReadTheDocs on each release.

## Immediate next steps
1. Capture these findings as GitHub issues (Qt4/modernization, pre-commit hook, NR dependencies, packaging automation, documentation pipeline).
2. Build minimal reproduction of `cmake` failure and document commands in the review file.
3. Outline a work plan for Qt6 + packaging, with a dependency graph for the roadmap items.
