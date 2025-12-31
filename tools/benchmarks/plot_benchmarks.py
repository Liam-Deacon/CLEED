#!/usr/bin/env python3
import argparse
import csv
import json
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError as exc:  # pragma: no cover - optional dependency
    raise SystemExit("matplotlib is required for plotting (pip install matplotlib)") from exc


def load_trace(trace_path: Path):
    points = []
    with trace_path.open("r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            try:
                points.append((int(row["eval"]), float(row["rtotal"])))
            except (KeyError, ValueError):
                continue
    return points


def best_so_far(points):
    best = []
    current = None
    for eval_idx, value in points:
        if current is None or value < current:
            current = value
        best.append((eval_idx, current))
    return best


def plot_dataset(name, rows, output_dir):
    fig, ax = plt.subplots(figsize=(8, 5))
    ax.set_title(f"{name} convergence")
    ax.set_xlabel("Evaluation")
    ax.set_ylabel("Best R")

    for row in rows:
        trace_file = row.get("trace_file")
        if not trace_file:
            continue
        trace_path = Path(trace_file)
        if not trace_path.exists():
            continue
        points = best_so_far(load_trace(trace_path))
        if not points:
            continue
        label = f"{row['optimizer']} (seed {row['seed']})"
        ax.plot([p[0] for p in points], [p[1] for p in points], alpha=0.7, label=label)

    ax.legend(fontsize="small")
    fig.tight_layout()
    output_dir.mkdir(parents=True, exist_ok=True)
    outfile = output_dir / f"{name}_convergence.png"
    fig.savefig(outfile, dpi=150)
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description="Plot CSEARCH benchmark traces.")
    parser.add_argument("--summary", required=True, help="Path to summary.json")
    parser.add_argument("--output-dir", default="benchmarks/plots")

    args = parser.parse_args()
    summary_path = Path(args.summary)
    output_dir = Path(args.output_dir)

    with summary_path.open("r", encoding="utf-8") as handle:
        results = json.load(handle)

    by_dataset = {}
    for row in results:
        by_dataset.setdefault(row["dataset"], []).append(row)

    for dataset, rows in by_dataset.items():
        plot_dataset(dataset, rows, output_dir)

    print(f"Plots written to {output_dir}")


if __name__ == "__main__":
    main()
