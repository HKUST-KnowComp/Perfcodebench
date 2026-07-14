#!/usr/bin/env python3
"""Run PerfCodeBench evaluation across multiple configured models."""

from __future__ import annotations

import argparse
import json
import subprocess
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONFIG_PATH = ROOT / "configs.json"
RESULTS_DIR = ROOT / "results"


def load_model_aliases() -> list[str]:
    return list(json.loads(CONFIG_PATH.read_text()).keys())


def sanitize(name: str) -> str:
    return name.replace("/", "__")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--models", nargs="*", default=None)
    parser.add_argument("--exclude-models", nargs="*", default=[])
    parser.add_argument("--skip-existing", action="store_true")
    parser.add_argument("--limit", type=int, default=None)
    parser.add_argument("--start", type=int, default=0)
    parser.add_argument("--task-list", type=Path, default=None)
    parser.add_argument("--cpu-jobs", type=int, default=128)
    parser.add_argument("--cuda-jobs", type=int, default=4)
    parser.add_argument("--rerun-cpu-jobs", type=int, default=32)
    parser.add_argument("--rerun-cuda-jobs", type=int, default=1)
    parser.add_argument("--fast-runs", type=int, default=1)
    parser.add_argument("--full-runs", type=int, default=3)
    parser.add_argument("--task-timeout-sec", type=int, default=240)
    parser.add_argument("--timeout-multiplier", type=float, default=1.5)
    parser.add_argument("--timeout-min-sec", type=int, default=5)
    parser.add_argument("--timeout-max-sec", type=int, default=120)
    parser.add_argument("--baseline-hard-timeout-sec", type=int, default=600)
    parser.add_argument("--prepare-timeouts-max-rounds", type=int, default=50)
    parser.add_argument("--prepare-timeouts-retry-sleep-sec", type=int, default=2)
    parser.add_argument("--results-prefix", default=None)
    args = parser.parse_args()

    aliases = args.models or load_model_aliases()
    aliases = [alias for alias in aliases if alias not in set(args.exclude_models)]

    timeout_json = RESULTS_DIR / "benchmark_timeout_table.json"
    timeout_jsonl = RESULTS_DIR / "benchmark_timeout_table.jsonl"

    for alias in aliases:
        stamp = time.strftime("%Y%m%d_%H%M%S")
        result_dir = RESULTS_DIR / f"{(args.results_prefix or sanitize(alias))}_eval_{stamp}"
        if args.skip_existing:
            existing = sorted(RESULTS_DIR.glob(f"{sanitize(alias)}_eval_*"))
            if existing:
                print(f"[skip-existing] {alias}: found {existing[-1].name}", flush=True)
                continue

        cmd = [
            "python3",
            "scripts/run_model_suite.py",
            "--model",
            alias,
            "--prepare-timeouts",
            "--require-complete-timeout-table",
            "--prepare-timeouts-max-rounds",
            str(args.prepare_timeouts_max_rounds),
            "--prepare-timeouts-retry-sleep-sec",
            str(args.prepare_timeouts_retry_sleep_sec),
            "--timeout-table-jsonl",
            str(timeout_jsonl),
            "--benchmark-timeout-table",
            str(timeout_json),
            "--timeout-multiplier",
            str(args.timeout_multiplier),
            "--timeout-min-sec",
            str(args.timeout_min_sec),
            "--timeout-max-sec",
            str(args.timeout_max_sec),
            "--baseline-hard-timeout-sec",
            str(args.baseline_hard_timeout_sec),
            "--fast-runs",
            str(args.fast_runs),
            "--full-runs",
            str(args.full_runs),
            "--cpu-jobs",
            str(args.cpu_jobs),
            "--cuda-jobs",
            str(args.cuda_jobs),
            "--rerun-cpu-jobs",
            str(args.rerun_cpu_jobs),
            "--rerun-cuda-jobs",
            str(args.rerun_cuda_jobs),
            "--task-timeout-sec",
            str(args.task_timeout_sec),
            "--output-dir",
            str(result_dir),
        ]
        if args.task_list is not None:
            cmd.extend(["--task-list", str(args.task_list)])
        else:
            if args.start:
                cmd.extend(["--start", str(args.start)])
            if args.limit is not None:
                cmd.extend(["--limit", str(args.limit)])

        print(f"[start] {alias} -> {result_dir}", flush=True)
        proc = subprocess.run(cmd, cwd=ROOT, check=False)
        print(f"[done] {alias} exit={proc.returncode}", flush=True)


if __name__ == "__main__":
    main()
