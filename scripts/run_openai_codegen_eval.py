#!/usr/bin/env python3
"""Generate candidate code with an OpenAI model and benchmark it."""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import time
from pathlib import Path
from typing import Any

from openai import OpenAI
from openai import APIConnectionError

from executable_benchmark_lib import (
    ROOT,
    benchmark_variant,
    resolve_candidate_source_path,
    load_instance,
    read_text,
    resolve_api_settings,
    solution_filename,
    task_dir,
    variant_dir,
    write_candidate_source,
)


SCHEMA: dict[str, Any] = {
    "type": "object",
    "additionalProperties": False,
    "properties": {
        "summary": {"type": "string"},
        "solution_source": {"type": "string"},
    },
    "required": ["summary", "solution_source"],
}

REPO_PATH_RE = re.compile(r"/(?:[^/\s\"']+/)*(?:eficodebench|perfcodebench)/")
NVCC_PATH_RE = re.compile(r"/(?:[^/\s\"']+/)*cuda-12/bin/nvcc")


def load_timeout_table(path: Path | None) -> dict[str, Any]:
    if path is None or not path.exists():
        return {}
    return json.loads(path.read_text())


def anonymize_text(text: str) -> str:
    text = NVCC_PATH_RE.sub("nvcc", text)
    return REPO_PATH_RE.sub("", text)


def anonymize_value(value: Any) -> Any:
    if isinstance(value, str):
        return anonymize_text(value)
    if isinstance(value, list):
        return [anonymize_value(item) for item in value]
    if isinstance(value, dict):
        return {key: anonymize_value(item) for key, item in value.items()}
    return value


def resolve_benchmark_timeout_sec(timeout_table: dict[str, Any], task_id: str) -> int | None:
    row = timeout_table.get(task_id)
    if not isinstance(row, dict):
        return None
    timeout_sec = row.get("timeout_sec")
    if isinstance(timeout_sec, int) and timeout_sec > 0:
        return timeout_sec
    return None


def read_optional_interface(task_root: Path) -> str:
    for rel in [
        "harness/interface.h",
        "harness/interface.hpp",
        "harness/interface.txt",
    ]:
        path = task_root / rel
        if path.exists():
            return read_text(path)
    return ""


def build_prompt(task_id: str) -> str:
    instance = load_instance(task_id)
    task_root = task_dir(task_id)
    source_name = solution_filename(task_id)
    interface_text = read_optional_interface(task_root)
    allowed_includes = instance.get("allowed_external_includes", [])
    interface_block = ""
    if interface_text:
        interface_block = f"Interface / task contract:\n{interface_text}\n\n"
    return (
        "Optimize the following implementation for performance while preserving correctness.\n"
        f"Return a full, complete, compilable replacement for {source_name}.\n"
        "Do not change the externally required function signature or entrypoint used by the harness.\n"
        "You may use only the dependencies already compiled by this task.\n\n"
        f"Task ID: {task_id}\n"
        f"Title: {instance['title']}\n"
        f"Goal: {instance['goal']}\n"
        f"Metric: {instance['metric']}\n"
        f"Correctness rule: {instance['correctness_rule']}\n"
        f"Allowed external includes: {', '.join(allowed_includes)}\n\n"
        f"{interface_block}"
        f"Current baseline {source_name}:\n"
        f"{read_text(task_root / 'baseline' / source_name)}\n"
    )


def call_model(task_id: str, model: str | None) -> dict[str, Any]:
    api_key, base_url, default_model = resolve_api_settings(model)
    client = OpenAI(api_key=api_key, base_url=base_url)
    prompt = build_prompt(task_id)
    for attempt in range(4):
        try:
            response = client.responses.create(
                model=model or default_model,
                temperature=0,
                max_output_tokens=4000,
                input=[
                    {
                        "role": "system",
                        "content": [
                            {
                                "type": "input_text",
                                "text": "You are a performance engineer. Output JSON only.",
                            }
                        ],
                    },
                    {"role": "user", "content": [{"type": "input_text", "text": prompt}]},
                ],
                text={
                    "format": {
                        "type": "json_schema",
                        "name": "perfcodebench_codegen",
                        "strict": True,
                        "schema": SCHEMA,
                    },
                },
            )
            return json.loads(response.output_text)
        except APIConnectionError:
            if attempt == 3:
                raise
            time.sleep(2 * (attempt + 1))

    raise RuntimeError("unreachable")


def benchmark_with_status(
    task_id: str,
    variant: str,
    runs: int,
    timeout_sec: int | None = None,
) -> dict[str, Any]:
    try:
        use_result_cache = variant in {"baseline", "reference"}
        result = benchmark_variant(
            task_id,
            variant,
            runs=runs,
            use_result_cache=use_result_cache,
            timeout_sec=timeout_sec,
        )
        result["status"] = "ok"
        return result
    except TimeoutError as exc:
        return {
            "task_id": task_id,
            "variant": variant,
            "runs": runs,
            "status": "timeout",
            "error_type": type(exc).__name__,
            "error": str(exc),
        }
    except subprocess.TimeoutExpired as exc:
        return {
            "task_id": task_id,
            "variant": variant,
            "runs": runs,
            "status": "timeout",
            "error_type": type(exc).__name__,
            "error": str(exc),
        }
    except Exception as exc:  # noqa: BLE001
        return {
            "task_id": task_id,
            "variant": variant,
            "runs": runs,
            "status": "error",
            "error_type": type(exc).__name__,
            "error": str(exc),
        }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("task_id")
    parser.add_argument("--model", default=None)
    parser.add_argument("--runs", type=int, default=5)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--output-dir", type=Path, default=None)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--reuse-candidate", action="store_true")
    parser.add_argument("--benchmark-timeout-table", type=Path, default=None)
    args = parser.parse_args()
    os.environ["PERFCODEBENCH_CANDIDATE_MODEL"] = args.model or "default"

    if args.reuse_candidate:
        candidate_source_path = resolve_candidate_source_path(args.task_id, args.model)
        payload = {
            "summary": "Reused existing candidate source for rerun benchmarking.",
            "solution_source": read_text(candidate_source_path),
        }
    elif args.dry_run:
        payload = {
            "summary": "Dry run placeholder.",
            "solution_source": read_text(variant_dir(args.task_id, "baseline") / solution_filename(args.task_id)),
        }
    else:
        payload = call_model(args.task_id, args.model)

    out_dir = args.output_dir or (ROOT / "results")
    out_dir.mkdir(exist_ok=True)
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    output_path = args.output or out_dir / f"{(args.model or 'default').replace('/', '_')}_{args.task_id}_{timestamp}.json"

    timeout_table = load_timeout_table(args.benchmark_timeout_table)
    benchmark_timeout_sec = resolve_benchmark_timeout_sec(timeout_table, args.task_id)
    candidate_path = write_candidate_source(args.task_id, payload["solution_source"], args.model)
    baseline = benchmark_with_status(args.task_id, "baseline", runs=args.runs, timeout_sec=benchmark_timeout_sec)
    reference = benchmark_with_status(args.task_id, "reference", runs=args.runs, timeout_sec=benchmark_timeout_sec)
    candidate = benchmark_with_status(args.task_id, "candidate", runs=args.runs, timeout_sec=benchmark_timeout_sec)

    result = {
        "task_id": args.task_id,
        "model": args.model,
        "dry_run": args.dry_run,
        "candidate_path": str(candidate_path.relative_to(ROOT)),
        "benchmark_timeout_sec": benchmark_timeout_sec,
        "model_output_summary": payload["summary"],
        "model_output_solution_source": payload["solution_source"],
        "baseline": baseline,
        "reference": reference,
        "candidate": candidate,
    }
    result = anonymize_value(result)
    output_path.write_text(json.dumps(result, indent=2, ensure_ascii=True) + "\n")
    print(output_path)
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
