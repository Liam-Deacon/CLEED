# Benchmark Harness

This directory contains a lightweight benchmark harness for running csearch
across multiple datasets, optimizers, and RNG seeds.

## Manifest format

`manifest.json` is a simple JSON file with a `datasets` array. Each dataset
entry can include:

- `name`: short identifier for output folders.
- `description`: free-form notes.
- `input`: path to the `.inp` file.
- `bulk`: optional `.bul` file path.
- `control`: optional `.ctr` file path.
- `delta`: optional `-d` value for initial displacement.
- `extra_files`: optional list of additional files to copy into the run dir.

Paths are resolved relative to the manifest location.

## Running a sweep

```
python3 tools/benchmarks/run_benchmarks.py \
  --manifest benchmarks/manifest.json \
  --output-dir benchmarks/out \
  --optimizers si,sa,ps,de \
  --seeds 1,2,3 \
  --max-evals 2000 \
  --max-iters 200 \
  --leed /path/to/cleed \
  --rfac /path/to/crfac
```

The runner writes per-run logs, a `summary.json`, `summary.csv`, and a `trace.csv`
per run (parsed from `*.log` evaluation lines).

## Plotting

```
python3 tools/benchmarks/plot_benchmarks.py \
  --summary benchmarks/out/<run_id>/summary.json \
  --output-dir benchmarks/plots
```

Matplotlib is required for plotting (`pip install matplotlib`).

## Report template

Use `benchmarks/report_template.md` as a starting point for summarising results.
