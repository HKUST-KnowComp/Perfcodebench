#!/usr/bin/env python3
"""Helpers for executable PerfCodeBench-style benchmark tasks."""

from __future__ import annotations

import json
import os
import shutil
import statistics
import subprocess
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
TASKS_DIR = ROOT / "executable_tasks"
BENCHMARK_CACHE_DIR = ROOT / "results" / "benchmark_cache"
DEFAULT_BENCHMARK_TIMEOUT_SEC = 120


def load_env_file(path: Path) -> None:
    if not path.exists():
        return
    for raw_line in path.read_text().splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        os.environ[key.strip()] = value.strip().strip('"').strip("'")


def load_model_configs() -> dict[str, Any]:
    for rel in ["configs.json", "config.json"]:
        path = ROOT / rel
        if path.exists():
            return json.loads(path.read_text())
    raise FileNotFoundError("Could not find configs.json or config.json in repository root.")


def resolve_api_settings(model_name: str | None = None) -> tuple[str, str | None, str]:
    env_file = ROOT / ".env"
    load_env_file(env_file)
    configs = load_model_configs()
    if model_name is not None:
        model_cfg = configs.get(model_name)
        if model_cfg is None:
            raise KeyError(f"Model '{model_name}' not found in configs.json")
        api_key = os.environ[model_cfg["api_key"]]
        base_url = os.environ.get(model_cfg["api_base"])
        model = model_cfg["model"]
        return api_key, base_url, model

    api_key = os.environ["OPENAI_API_KEY_1"]
    base_url = os.environ.get("OPENAI_API_BASE_1")
    model = os.environ.get("OPENAI_MODEL", "gpt-4o")
    return api_key, base_url, model


def load_instance(task_id: str) -> dict[str, Any]:
    instance_path = TASKS_DIR / task_id / "instance.json"
    return json.loads(instance_path.read_text())


def solution_filename(task_id: str) -> str:
    instance = load_instance(task_id)
    return instance.get("solution_filename", "solution.cpp")


def task_dir(task_id: str) -> Path:
    return TASKS_DIR / task_id


def variant_dir(task_id: str, variant: str) -> Path:
    return task_dir(task_id) / variant


def sanitize_model_alias(model_name: str | None) -> str:
    if not model_name:
        return "default"
    return model_name.replace("/", "__")


def candidate_dir(task_id: str, model_name: str | None = None) -> Path:
    root = variant_dir(task_id, "candidate")
    if model_name is None:
        return root
    return root / sanitize_model_alias(model_name)


def read_text(path: Path) -> str:
    return path.read_text()


def resolve_candidate_source_path(task_id: str, model_name: str | None = None) -> Path:
    if model_name is not None:
        model_path = candidate_dir(task_id, model_name) / solution_filename(task_id)
        if model_path.exists():
            return model_path
    legacy_path = variant_dir(task_id, "candidate") / solution_filename(task_id)
    if legacy_path.exists():
        return legacy_path
    if model_name is not None:
        return candidate_dir(task_id, model_name) / solution_filename(task_id)
    return legacy_path


def write_candidate_source(task_id: str, source: str, model_name: str | None = None) -> Path:
    out = candidate_dir(task_id, model_name) / solution_filename(task_id)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(source)
    return out


def _expand(template: str, *, task_root: Path, variant_root: Path, build_root: Path) -> str:
    return template.format(
        root=str(ROOT),
        task_dir=str(task_root),
        variant_dir=str(variant_root),
        build_dir=str(build_root),
    )


def _expand_list(items: list[str], *, task_root: Path, variant_root: Path, build_root: Path) -> list[str]:
    return [_expand(item, task_root=task_root, variant_root=variant_root, build_root=build_root) for item in items]


def _result_cache_path(task_id: str, variant: str, runs: int) -> Path:
    return BENCHMARK_CACHE_DIR / f"{task_id}__{variant}__runs{runs}.json"


def _build_inputs(task_id: str, variant: str, build_root: Path) -> list[Path]:
    instance = load_instance(task_id)
    task_root = task_dir(task_id)
    if variant == "candidate":
        model_alias = os.environ.get("PERFCODEBENCH_CANDIDATE_MODEL")
        variant_root = resolve_candidate_source_path(task_id, model_alias).parent
    else:
        variant_root = variant_dir(task_id, variant)
    build_cfg = instance["build"]
    inputs: list[Path] = []
    for rel in build_cfg.get("copy_sources_to_build", []):
        inputs.append(Path(_expand(rel, task_root=task_root, variant_root=variant_root, build_root=build_root)))
    for rel in build_cfg.get("sources", []):
        inputs.append(Path(_expand(rel, task_root=task_root, variant_root=variant_root, build_root=build_root)))
    return [path for path in inputs if path.exists()]


def _is_fresh(binary_path: Path, inputs: list[Path]) -> bool:
    if not binary_path.exists():
        return False
    binary_mtime = binary_path.stat().st_mtime
    return all(path.stat().st_mtime <= binary_mtime for path in inputs)


def _result_inputs(task_id: str, variant: str) -> list[Path]:
    task_root = task_dir(task_id)
    paths = [task_root / "instance.json"]
    paths.extend(path for path in (task_root / "harness").rglob("*") if path.is_file())
    if variant == "candidate":
        model_alias = os.environ.get("PERFCODEBENCH_CANDIDATE_MODEL")
        candidate_root = candidate_dir(task_id)
        paths.extend(path for path in candidate_root.rglob("*") if path.is_file())
        resolved_path = resolve_candidate_source_path(task_id, model_alias)
        if resolved_path.is_file() and resolved_path not in paths:
            paths.append(resolved_path)
    else:
        paths.extend(path for path in (variant_dir(task_id, variant)).rglob("*") if path.is_file())
    return paths


def _load_cached_result(task_id: str, variant: str, runs: int) -> dict[str, Any] | None:
    cache_path = _result_cache_path(task_id, variant, runs)
    if not cache_path.exists():
        return None
    cache_mtime = cache_path.stat().st_mtime
    if any(path.stat().st_mtime > cache_mtime for path in _result_inputs(task_id, variant)):
        return None
    return json.loads(cache_path.read_text())


def _store_cached_result(task_id: str, variant: str, runs: int, result: dict[str, Any]) -> None:
    BENCHMARK_CACHE_DIR.mkdir(parents=True, exist_ok=True)
    cache_path = _result_cache_path(task_id, variant, runs)
    cache_path.write_text(json.dumps(result, indent=2, ensure_ascii=True) + "\n")


def build_variant(task_id: str, variant: str) -> Path:
    instance = load_instance(task_id)
    task_root = task_dir(task_id)
    if variant == "candidate":
        model_alias = os.environ.get("PERFCODEBENCH_CANDIDATE_MODEL")
        variant_root = resolve_candidate_source_path(task_id, model_alias).parent
    else:
        variant_root = variant_dir(task_id, variant)
    build_root = ROOT / "build" / task_id / variant
    build_root.mkdir(parents=True, exist_ok=True)
    binary_path = build_root / "bench"

    build_cfg = instance["build"]
    if _is_fresh(binary_path, _build_inputs(task_id, variant, build_root)):
        return binary_path
    if build_cfg.get("copy_sources_to_build"):
        for rel in build_cfg["copy_sources_to_build"]:
            src = Path(_expand(rel, task_root=task_root, variant_root=variant_root, build_root=build_root))
            shutil.copy2(src, build_root / src.name)
    if build_cfg.get("skip"):
        return binary_path
    if "command" in build_cfg:
        args = _expand_list(build_cfg["command"], task_root=task_root, variant_root=variant_root, build_root=build_root)
        subprocess.run(args, check=True, cwd=ROOT)
        return binary_path
    compiler = build_cfg["compiler"]
    args = [compiler]
    args.extend(_expand(flag, task_root=task_root, variant_root=variant_root, build_root=build_root) for flag in build_cfg["cxxflags"])
    args.extend(_expand(src, task_root=task_root, variant_root=variant_root, build_root=build_root) for src in build_cfg["sources"])
    args.extend(["-o", str(binary_path)])
    subprocess.run(args, check=True, cwd=ROOT)
    return binary_path


def run_once(
    task_id: str,
    variant: str,
    binary_path: Path | None = None,
    timeout_sec: int | None = None,
) -> dict[str, Any]:
    if binary_path is None:
        binary_path = build_variant(task_id, variant)
    instance = load_instance(task_id)
    task_root = task_dir(task_id)
    variant_root = variant_dir(task_id, variant)
    build_root = ROOT / "build" / task_id / variant
    run_cfg = instance.get("run")
    if run_cfg and "argv" in run_cfg:
        argv = _expand_list(run_cfg["argv"], task_root=task_root, variant_root=variant_root, build_root=build_root)
    else:
        argv = [str(binary_path)]
    proc = subprocess.run(
        argv,
        check=True,
        capture_output=True,
        text=True,
        cwd=ROOT,
        timeout=timeout_sec if timeout_sec is not None else DEFAULT_BENCHMARK_TIMEOUT_SEC,
    )
    return json.loads(proc.stdout)


def benchmark_variant(
    task_id: str,
    variant: str,
    runs: int = 5,
    *,
    use_result_cache: bool = False,
    timeout_sec: int | None = None,
) -> dict[str, Any]:
    if use_result_cache:
        cached = _load_cached_result(task_id, variant, runs)
        if cached is not None:
            return cached
    binary_path = build_variant(task_id, variant)
    outputs = [
        run_once(task_id, variant, binary_path=binary_path, timeout_sec=timeout_sec)
        for _ in range(runs)
    ]
    elapsed = [row["elapsed_ns"] for row in outputs]
    result = {
        "task_id": task_id,
        "variant": variant,
        "runs": runs,
        "median_elapsed_ns": int(statistics.median(elapsed)),
        "min_elapsed_ns": min(elapsed),
        "max_elapsed_ns": max(elapsed),
        "all_elapsed_ns": elapsed,
        "all_ok": all(row["ok"] for row in outputs),
        "sample_output": outputs[0],
    }
    if use_result_cache:
        _store_cached_result(task_id, variant, runs, result)
    return result
