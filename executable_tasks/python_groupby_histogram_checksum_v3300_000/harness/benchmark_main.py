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


def make_input(n: int, buckets: int):
    keys = [0] * n
    vals = [0] * n
    state = 0xA55A5AA55A5AA55A
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        keys[i] = (state >> 16) % buckets
        vals[i] = 1 + ((state >> 40) & 31)
    return keys, vals


def expected_checksum(keys, vals, buckets: int) -> int:
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v
    h = 1469598103934665603
    for v in hist:
        h ^= v + 1
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def main() -> None:
    keys, vals = make_input(12000, 128)
    expected = expected_checksum(keys, vals, 128)
    start = time.perf_counter_ns()
    actual = solution.run(keys, vals, 128, 8)
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
