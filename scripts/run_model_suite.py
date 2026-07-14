#!/usr/bin/env python3
"""Run a model across executable tasks, optionally in parallel."""

from __future__ import annotations

import argparse
import concurrent.futures
import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TASKS_DIR = ROOT / "executable_tasks"
LOGS_DIR = ROOT / "results" / "logs"


def is_cuda_task(task_id: str) -> bool:
    return task_id.startswith("cuda_")


def count_timeout_table_entries(path: Path) -> int:
    if not path.exists():
        return 0
    data = json.loads(path.read_text())
    return len(data)


def run_prepare_timeouts_until_complete(
    prep_cmd: list[str],
    timeout_table_path: Path,
    target_count: int,
    *,
    max_rounds: int,
    retry_sleep_sec: int,
) -> None:
    rounds_without_progress = 0
    last_count = count_timeout_table_entries(timeout_table_path)
    for round_idx in range(1, max_rounds + 1):
        if last_count >= target_count:
            print(f"Timeout table complete before round {round_idx}: {last_count} / {target_count}", flush=True)
            return
        print(
            f"Prepare timeouts round {round_idx}: {last_count} / {target_count}",
            flush=True,
        )
        proc = subprocess.run(prep_cmd, cwd=ROOT, check=False)
        new_count = count_timeout_table_entries(timeout_table_path)
        print(
            f"Prepare timeouts round {round_idx} finished with exit={proc.returncode}; "
            f"entries {new_count} / {target_count}",
            flush=True,
        )
        if new_count >= target_count:
            return
        if new_count > last_count:
            rounds_without_progress = 0
        else:
            rounds_without_progress += 1
        last_count = new_count
        if retry_sleep_sec > 0:
            subprocess.run(["sleep", str(retry_sleep_sec)], cwd=ROOT, check=False)
        if rounds_without_progress >= 3:
            raise RuntimeError(
                f"Timeout table stalled at {new_count} / {target_count} for 3 rounds"
            )
    raise RuntimeError(
        f"Timeout table incomplete after {max_rounds} rounds: {last_count} / {target_count}"
    )


def run_task(
    task_id: str,
    model: str,
    runs: int,
    output_dir: Path | None = None,
    reuse_candidate: bool = False,
    timeout_sec: int | None = None,
    benchmark_timeout_table: Path | None = None,
) -> tuple[str, int]:
    cmd = [
        "python3",
        "scripts/run_openai_codegen_eval.py",
        task_id,
        "--model",
        model,
        "--runs",
        str(runs),
    ]
    if output_dir is not None:
        cmd.extend(["--output-dir", str(output_dir)])
    if reuse_candidate:
        cmd.append("--reuse-candidate")
    if benchmark_timeout_table is not None:
        cmd.extend(["--benchmark-timeout-table", str(benchmark_timeout_table)])
    proc = subprocess.run(
        cmd,
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        timeout=timeout_sec,
    )
    LOGS_DIR.mkdir(parents=True, exist_ok=True)
    log_path = LOGS_DIR / f"{model.replace('/', '_')}_{task_id}.log"
    log_path.write_text(proc.stdout + ("\n" if proc.stdout and proc.stderr else "") + proc.stderr)
    return task_id, proc.returncode


def run_many(
    tasks: list[str],
    model: str,
    runs: int,
    jobs: int,
    output_dir: Path | None = None,
    reuse_candidate: bool = False,
    timeout_sec: int | None = None,
    benchmark_timeout_table: Path | None = None,
) -> None:
    if jobs <= 1:
        for task_id in tasks:
            print(f"=== {task_id} ===", flush=True)
            try:
                _, code = run_task(
                    task_id,
                    model,
                    runs,
                    output_dir=output_dir,
                    reuse_candidate=reuse_candidate,
                    timeout_sec=timeout_sec,
                    benchmark_timeout_table=benchmark_timeout_table,
                )
                status = "ok" if code == 0 else f"exit={code}"
            except subprocess.TimeoutExpired:
                status = "timeout"
            print(f"[{task_id}] {status}", flush=True)
        return

    print(
        f"Running {len(tasks)} tasks with jobs={jobs} model={model} runs={runs}",
        flush=True,
    )
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        future_map = {
            pool.submit(
                run_task,
                task_id,
                model,
                runs,
                output_dir,
                reuse_candidate,
                timeout_sec,
                benchmark_timeout_table,
            ): task_id
            for task_id in tasks
        }
        for future in concurrent.futures.as_completed(future_map):
            task_id = future_map[future]
            try:
                _, code = future.result()
                status = "ok" if code == 0 else f"exit={code}"
            except subprocess.TimeoutExpired:
                status = "timeout"
            except Exception as exc:  # noqa: BLE001
                status = f"error={type(exc).__name__}: {exc}"
            print(f"[{task_id}] {status}", flush=True)


def run_split_pools(
    tasks: list[str],
    model: str,
    runs: int,
    cpu_jobs: int,
    cuda_jobs: int,
    output_dir: Path | None = None,
    reuse_candidate: bool = False,
    timeout_sec: int | None = None,
    benchmark_timeout_table: Path | None = None,
) -> None:
    cpu_tasks = [task_id for task_id in tasks if not is_cuda_task(task_id)]
    cuda_tasks = [task_id for task_id in tasks if is_cuda_task(task_id)]

    if cpu_tasks:
        print(f"CPU pool: {len(cpu_tasks)} tasks jobs={cpu_jobs} runs={runs}", flush=True)
        run_many(
            cpu_tasks,
            model,
            runs,
            cpu_jobs,
            output_dir=output_dir,
            reuse_candidate=reuse_candidate,
            timeout_sec=timeout_sec,
            benchmark_timeout_table=benchmark_timeout_table,
        )
    if cuda_tasks:
        print(f"CUDA pool: {len(cuda_tasks)} tasks jobs={cuda_jobs} runs={runs}", flush=True)
        run_many(
            cuda_tasks,
            model,
            runs,
            cuda_jobs,
            output_dir=output_dir,
            reuse_candidate=reuse_candidate,
            timeout_sec=timeout_sec,
            benchmark_timeout_table=benchmark_timeout_table,
        )


def load_result(path: Path) -> dict | None:
    if not path.exists():
        return None
    return json.loads(path.read_text())


def comparable_and_promising(result: dict, threshold: float) -> bool:
    cand = result.get("candidate", {})
    base = result.get("baseline", {})
    ref = result.get("reference", {})
    if cand.get("status") != "ok" or not cand.get("all_ok", False):
        return False
    if base.get("status") != "ok" or ref.get("status") != "ok":
        return False
    if not base.get("all_ok", False) or not ref.get("all_ok", False):
        return False
    b = base["median_elapsed_ns"]
    r = ref["median_elapsed_ns"]
    c = cand["median_elapsed_ns"]
    if c < b:
        return True
    denom = b - r
    if denom <= 0:
        return False
    cgre = max(0.0, min(1.0, (b - c) / denom))
    return cgre >= threshold


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", required=True)
    parser.add_argument("--runs", type=int, default=3)
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--start", type=int, default=0)
    parser.add_argument("--limit", type=int, default=None)
    parser.add_argument("--task-list", type=Path, default=None)
    parser.add_argument("--fast-runs", type=int, default=None)
    parser.add_argument("--full-runs", type=int, default=None)
    parser.add_argument("--rerun-jobs", type=int, default=None)
    parser.add_argument("--rerun-threshold", type=float, default=0.8)
    parser.add_argument("--output-dir", type=Path, default=None)
    parser.add_argument("--cpu-jobs", type=int, default=None)
    parser.add_argument("--cuda-jobs", type=int, default=None)
    parser.add_argument("--rerun-cpu-jobs", type=int, default=None)
    parser.add_argument("--rerun-cuda-jobs", type=int, default=None)
    parser.add_argument("--task-timeout-sec", type=int, default=240)
    parser.add_argument("--benchmark-timeout-table", type=Path, default=ROOT / "results" / "benchmark_timeout_table.json")
    parser.add_argument("--prepare-timeouts", action="store_true")
    parser.add_argument("--timeout-multiplier", type=float, default=1.5)
    parser.add_argument("--timeout-min-sec", type=int, default=5)
    parser.add_argument("--timeout-max-sec", type=int, default=120)
    parser.add_argument("--baseline-hard-timeout-sec", type=int, default=600)
    parser.add_argument("--timeout-table-jsonl", type=Path, default=None)
    parser.add_argument("--timeout-flush-every", type=int, default=25)
    parser.add_argument("--no-timeout-resume", action="store_true")
    parser.add_argument("--require-complete-timeout-table", action="store_true")
    parser.add_argument("--prepare-timeouts-max-rounds", type=int, default=50)
    parser.add_argument("--prepare-timeouts-retry-sleep-sec", type=int, default=2)
    args = parser.parse_args()

    if args.task_list is not None:
        tasks = [line.strip() for line in args.task_list.read_text().splitlines() if line.strip()]
    else:
        tasks = sorted(p.parent.name for p in TASKS_DIR.glob("*/instance.json"))
    if args.start:
        tasks = tasks[args.start :]
    if args.limit is not None:
        tasks = tasks[: args.limit]
    if args.prepare_timeouts:
        prep_cmd = [
            "python3",
            "scripts/prepare_timeout_table.py",
            "--runs",
            "1",
            "--multiplier",
            str(args.timeout_multiplier),
            "--min-timeout-sec",
            str(args.timeout_min_sec),
            "--max-timeout-sec",
            str(args.timeout_max_sec),
            "--baseline-hard-timeout-sec",
            str(args.baseline_hard_timeout_sec),
            "--output",
            str(args.benchmark_timeout_table),
            "--flush-every",
            str(args.timeout_flush_every),
        ]
        if args.timeout_table_jsonl is not None:
            prep_cmd.extend(["--jsonl-output", str(args.timeout_table_jsonl)])
        if args.no_timeout_resume:
            prep_cmd.append("--no-resume")
        if args.task_list is not None:
            prep_cmd.extend(["--task-list", str(args.task_list)])
        else:
            if args.start:
                prep_cmd.extend(["--start", str(args.start)])
            if args.limit is not None:
                prep_cmd.extend(["--limit", str(args.limit)])
        run_prepare_timeouts_until_complete(
            prep_cmd,
            args.benchmark_timeout_table,
            len(tasks),
            max_rounds=args.prepare_timeouts_max_rounds,
            retry_sleep_sec=args.prepare_timeouts_retry_sleep_sec,
        )
    if args.require_complete_timeout_table:
        table_count = count_timeout_table_entries(args.benchmark_timeout_table)
        if table_count < len(tasks):
            raise RuntimeError(
                f"Timeout table incomplete: {table_count} / {len(tasks)} entries in {args.benchmark_timeout_table}"
            )
    output_dir = args.output_dir
    cpu_jobs = args.cpu_jobs or args.jobs
    cuda_jobs = args.cuda_jobs or args.jobs
    if args.fast_runs is None or args.full_runs is None:
        if args.cpu_jobs is not None or args.cuda_jobs is not None:
            run_split_pools(
                tasks,
                args.model,
                args.runs,
                cpu_jobs,
                cuda_jobs,
                output_dir=output_dir,
                timeout_sec=args.task_timeout_sec,
                benchmark_timeout_table=args.benchmark_timeout_table,
            )
        else:
            run_many(
                tasks,
                args.model,
                args.runs,
                args.jobs,
                output_dir=output_dir,
                timeout_sec=args.task_timeout_sec,
                benchmark_timeout_table=args.benchmark_timeout_table,
            )
        return

    fast_dir = output_dir or (ROOT / "results" / f"{args.model.replace('/', '_')}_fastpass")
    rerun_dir = fast_dir
    if args.cpu_jobs is not None or args.cuda_jobs is not None:
        run_split_pools(
            tasks,
            args.model,
            args.fast_runs,
            cpu_jobs,
            cuda_jobs,
            output_dir=fast_dir,
            timeout_sec=args.task_timeout_sec,
            benchmark_timeout_table=args.benchmark_timeout_table,
        )
    else:
        run_many(
            tasks,
            args.model,
            args.fast_runs,
            args.jobs,
            output_dir=fast_dir,
            timeout_sec=args.task_timeout_sec,
            benchmark_timeout_table=args.benchmark_timeout_table,
        )

    rerun_tasks: list[str] = []
    for task_id in tasks:
        candidates = sorted(fast_dir.glob(f"{args.model.replace('/', '_')}_{task_id}_*.json"))
        if not candidates:
            continue
        result = load_result(candidates[-1])
        if result is not None and comparable_and_promising(result, args.rerun_threshold):
            rerun_tasks.append(task_id)

    rerun_jobs = args.rerun_jobs or args.jobs
    rerun_cpu_jobs = args.rerun_cpu_jobs or args.cpu_jobs or rerun_jobs
    rerun_cuda_jobs = args.rerun_cuda_jobs or args.cuda_jobs or rerun_jobs
    print(
        f"Selective rerun on {len(rerun_tasks)} / {len(tasks)} tasks with jobs={rerun_jobs} runs={args.full_runs}",
        flush=True,
    )
    if args.rerun_cpu_jobs is not None or args.rerun_cuda_jobs is not None or args.cpu_jobs is not None or args.cuda_jobs is not None:
        run_split_pools(
            rerun_tasks,
            args.model,
            args.full_runs,
            rerun_cpu_jobs,
            rerun_cuda_jobs,
            output_dir=rerun_dir,
            reuse_candidate=True,
            timeout_sec=args.task_timeout_sec,
            benchmark_timeout_table=args.benchmark_timeout_table,
        )
    else:
        run_many(
            rerun_tasks,
            args.model,
            args.full_runs,
            rerun_jobs,
            output_dir=rerun_dir,
            reuse_candidate=True,
            timeout_sec=args.task_timeout_sec,
            benchmark_timeout_table=args.benchmark_timeout_table,
        )


if __name__ == "__main__":
    main()
