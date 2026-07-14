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

def make_deltas(n: int) -> list[int]:
    deltas = [0] * n
    state = 0xCAFEBABE13572468
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        deltas[i] = ((state >> 24) & 1023) - 348
    return deltas


def expected_sum(deltas: list[int]) -> int:
    current = 0
    total = 0
    for d in deltas:
        current += d
        total += current + 0x9E3779B1
    return total


def main() -> None:
    deltas = make_deltas(366800)
    expected = expected_sum(deltas)
    start = time.perf_counter_ns()
    actual = solution.run(deltas, 18)
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
