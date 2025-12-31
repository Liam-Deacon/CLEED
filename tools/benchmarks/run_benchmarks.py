#!/usr/bin/env python3
import argparse
import csv
import json
import os
import re
import shutil
import subprocess
import time
from pathlib import Path

RMIN_RE = re.compile(r"rmin\s*=\s*([-+0-9.eE]+)")
TRACE_RE = re.compile(r"^#\s*(\d+)\b.*?rt:([0-9.+-Ee]+)")
EVAL_RE = re.compile(r"No\. of .*?:\s*(\d+)")


def parse_log(log_path: Path):
    rmin = None
    reported_iters = None
    trace = []
    if not log_path.exists():
        return rmin, reported_iters, trace
    with log_path.open("r", encoding="utf-8", errors="replace") as handle:
        for line in handle:
            match = RMIN_RE.search(line)
            if match:
                try:
                    rmin = float(match.group(1))
                except ValueError:
                    pass
            match = TRACE_RE.search(line)
            if match:
                try:
                    eval_idx = int(match.group(1))
                    rtotal = float(match.group(2))
                    trace.append((eval_idx, rtotal))
                except ValueError:
                    pass
            match = EVAL_RE.search(line)
            if match:
                try:
                    reported_iters = int(match.group(1))
                except ValueError:
                    pass
    return rmin, reported_iters, trace


def write_trace_csv(trace, output_path: Path):
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["eval", "rtotal"])
        writer.writerows(trace)


def load_manifest(path: Path):
    with path.open("r", encoding="utf-8") as handle:
        data = json.load(handle)
    datasets = data.get("datasets", [])
    if not datasets:
        raise ValueError("manifest contains no datasets")
    base_dir = path.parent
    for dataset in datasets:
        for key in ("input", "bulk", "control"):
            value = dataset.get(key)
            if value:
                dataset[key] = str((base_dir / value).resolve())
        extra = dataset.get("extra_files", [])
        dataset["extra_files"] = [str((base_dir / p).resolve()) for p in extra]
    return datasets


def parse_seeds(args):
    if args.seed_start is not None:
        if args.seed_count is None or args.seed_count <= 0:
            raise ValueError("--seed-count must be positive with --seed-start")
        return list(range(args.seed_start, args.seed_start + args.seed_count))
    seeds = []
    for token in args.seeds.split(","):
        token = token.strip()
        if not token:
            continue
        seeds.append(int(token))
    return seeds


def ensure_program(name: str, path):
    if path:
        return path
    value = os.environ.get(name)
    if not value:
        raise ValueError(f"{name} is not set; pass --{name.lower()} or export it")
    return value


def run_benchmarks(args):
    manifest_path = Path(args.manifest).resolve()
    datasets = load_manifest(manifest_path)

    seeds = parse_seeds(args)
    optimizers = [opt.strip() for opt in args.optimizers.split(",") if opt.strip()]

    if not optimizers:
        raise ValueError("no optimizers specified")
    if not seeds:
        raise ValueError("no seeds specified")

    csearch_bin = args.csearch
    leed_bin = ensure_program("CSEARCH_LEED", args.leed)
    rfac_bin = ensure_program("CSEARCH_RFAC", args.rfac)

    run_id = time.strftime("run_%Y%m%d_%H%M%S")
    output_root = Path(args.output_dir).resolve() / run_id
    output_root.mkdir(parents=True, exist_ok=True)
    shutil.copy2(manifest_path, output_root / manifest_path.name)

    results = []

    for dataset in datasets:
        dataset_name = dataset.get("name") or Path(dataset["input"]).stem
        input_path = Path(dataset["input"])
        if not input_path.exists():
            raise FileNotFoundError(input_path)

        for optimizer in optimizers:
            for seed in seeds:
                run_dir = output_root / dataset_name / optimizer / f"seed_{seed}"
                run_dir.mkdir(parents=True, exist_ok=True)

                files_to_copy = [dataset["input"]]
                if dataset.get("bulk"):
                    files_to_copy.append(dataset["bulk"])
                if dataset.get("control"):
                    files_to_copy.append(dataset["control"])
                files_to_copy.extend(dataset.get("extra_files", []))

                for file_path in files_to_copy:
                    src = Path(file_path)
                    if not src.exists():
                        raise FileNotFoundError(src)
                    shutil.copy2(src, run_dir / src.name)

                input_name = input_path.name
                cmd = [
                    csearch_bin,
                    "-i",
                    input_name,
                    "-s",
                    optimizer,
                    "--seed",
                    str(seed),
                ]
                if dataset.get("delta") is not None:
                    cmd.extend(["-d", str(dataset["delta"])])
                if args.max_evals:
                    cmd.extend(["--max-evals", str(args.max_evals)])
                if args.max_iters:
                    cmd.extend(["--max-iters", str(args.max_iters)])

                env = os.environ.copy()
                env["CSEARCH_LEED"] = leed_bin
                env["CSEARCH_RFAC"] = rfac_bin

                start = time.time()
                if args.dry_run:
                    exit_code = 0
                    stdout = ""
                    stderr = ""
                else:
                    proc = subprocess.run(
                        cmd,
                        cwd=run_dir,
                        env=env,
                        text=True,
                        capture_output=True,
                        timeout=args.timeout,
                    )
                    exit_code = proc.returncode
                    stdout = proc.stdout
                    stderr = proc.stderr
                elapsed = time.time() - start

                (run_dir / "run.stdout").write_text(stdout, encoding="utf-8")
                (run_dir / "run.stderr").write_text(stderr, encoding="utf-8")

                project = input_path.stem
                log_path = run_dir / f"{project}.log"
                rmin, reported_iters, trace = parse_log(log_path)

                trace_path = run_dir / "trace.csv"
                if trace:
                    write_trace_csv(trace, trace_path)
                else:
                    trace_path = None

                evals = trace[-1][0] if trace else None

                results.append(
                    {
                        "dataset": dataset_name,
                        "optimizer": optimizer,
                        "seed": seed,
                        "status": "ok" if exit_code == 0 else "error",
                        "exit_code": exit_code,
                        "elapsed_seconds": round(elapsed, 3),
                        "rmin": rmin,
                        "evals": evals,
                        "reported_iters": reported_iters,
                        "log_file": str(log_path),
                        "trace_file": str(trace_path) if trace_path else None,
                        "run_dir": str(run_dir),
                        "command": " ".join(cmd),
                    }
                )

    summary_json = output_root / "summary.json"
    summary_csv = output_root / "summary.csv"

    summary_json.write_text(json.dumps(results, indent=2), encoding="utf-8")

    with summary_csv.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=[
                "dataset",
                "optimizer",
                "seed",
                "status",
                "exit_code",
                "elapsed_seconds",
                "rmin",
                "evals",
                "reported_iters",
                "log_file",
                "trace_file",
                "run_dir",
                "command",
            ],
        )
        writer.writeheader()
        writer.writerows(results)

    return output_root


def main():
    parser = argparse.ArgumentParser(description="Run CSEARCH benchmark sweeps.")
    parser.add_argument("--manifest", default="benchmarks/manifest.json")
    parser.add_argument("--output-dir", default="benchmarks/out")
    parser.add_argument("--optimizers", default="si,sa,ps,de")
    parser.add_argument("--seeds", default="1")
    parser.add_argument("--seed-start", type=int)
    parser.add_argument("--seed-count", type=int)
    parser.add_argument("--max-evals", type=int)
    parser.add_argument("--max-iters", type=int)
    parser.add_argument("--csearch", default="csearch")
    parser.add_argument("--leed", help="Path to LEED program (CSEARCH_LEED)")
    parser.add_argument("--rfac", help="Path to R factor program (CSEARCH_RFAC)")
    parser.add_argument("--timeout", type=int)
    parser.add_argument("--dry-run", action="store_true")

    args = parser.parse_args()

    try:
        output_root = run_benchmarks(args)
    except Exception as exc:
        raise SystemExit(f"error: {exc}")

    print(f"Benchmark results written to {output_root}")


if __name__ == "__main__":
    main()
