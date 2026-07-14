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


def make_scores(n: int) -> list[int]:
    scores = [0] * n
    state = 0x3141592653589793
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        scores[i] = (state >> 16) & 0xFFFFF
    return scores


def checksum_topk(scores: list[int], k: int) -> int:
    pairs = sorted(((s, i) for i, s in enumerate(scores)), key=lambda x: (-x[0], x[1]))
    h = 1469598103934665603
    for score, index in pairs[:k]:
        h ^= ((score << 32) ^ index)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def main() -> None:
    scores = make_scores(149700)
    expected = checksum_topk(scores, 20)
    start = time.perf_counter_ns()
    actual = solution.run(scores, 20, 27)
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
