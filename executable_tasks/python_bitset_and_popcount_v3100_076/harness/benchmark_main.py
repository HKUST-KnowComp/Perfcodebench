import importlib.util
import json
import os
import time
from pathlib import Path


def load_solution():
    solution_path = os.environ["PERFCODEBENCH_SOLUTION"]
    spec = importlib.util.spec_from_file_location("perfcodebench_solution", solution_path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module


solution = load_solution()

def make_words(n: int) -> tuple[list[int], list[int]]:
    left = [0] * n
    right = [0] * n
    state = 0xDEADBEEF12345678
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        left[i] = state ^ ((state << 7) & ((1 << 64) - 1))
        state = (state * 2862933555777941757 + 3037000493) & ((1 << 64) - 1)
        right[i] = state ^ (state >> 11)
    return left, right


def expected_count(left: list[int], right: list[int]) -> int:
    total = 0
    for a, b in zip(left, right):
        total += (a & b).bit_count()
    return total


def main() -> None:
    left, right = make_words(131200)
    expected = expected_count(left, right)
    start = time.perf_counter_ns()
    actual = solution.run(left, right, 24)
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
