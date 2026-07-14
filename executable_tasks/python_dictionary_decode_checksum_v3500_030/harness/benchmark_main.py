import importlib.util
import json
import os
import time


def load_solution():
    solution_path = os.environ["PERFCODEBENCH_SOLUTION"]
    spec = importlib.util.spec_from_file_location("perfcodebench_solution", solution_path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module


solution = load_solution()


def make_input(n: int, dict_size: int):
    ids = [0] * n
    dict_vals = [0] * dict_size
    state = 0x9E3779B97F4A7C15
    for i in range(dict_size):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        dict_vals[i] = (state >> 16) & 0xFFFFFF
    for i in range(n):
        state = (state * 2862933555777941757 + 3037000493) & ((1 << 64) - 1)
        ids[i] = (state >> 20) % dict_size
    return ids, dict_vals


def expected_checksum(ids, dict_vals):
    h = 1469598103934665603
    for idx in ids:
        h ^= dict_vals[idx] + 1
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def main() -> None:
    ids, dict_vals = make_input(155000, 1024)
    expected = expected_checksum(ids, dict_vals)
    start = time.perf_counter_ns()
    actual = solution.run(ids, dict_vals, 8)
    elapsed = time.perf_counter_ns() - start
    print(json.dumps({
        "ok": actual == expected,
        "elapsed_ns": elapsed,
        "expected_checksum": expected,
        "actual_checksum": actual,
    }))
    if actual != expected:
        raise SystemExit("checksum mismatch")


if __name__ == "__main__":
    main()
