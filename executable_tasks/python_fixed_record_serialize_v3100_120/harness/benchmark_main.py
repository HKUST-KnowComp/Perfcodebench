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

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def make_input(n: int):
    ids = [0] * n
    ts = [0] * n
    levels = [0] * n
    lens = [0] * n
    state = 0x13579BDF2468ACE0
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        ids[i] = state & 0xFFFFFFFF
        state = (state * 2862933555777941757 + 3037000493) & ((1 << 64) - 1)
        ts[i] = state & 0xFFFFFFFF
        levels[i] = (state >> 16) & 7
        lens[i] = 24 + ((state >> 24) & 255)
    return ids, ts, levels, lens


def expected_checksum(ids, ts, levels, lens) -> int:
    out = bytearray(len(ids) * 12)
    ptr = 0
    for i in range(len(ids)):
        out[ptr:ptr+4] = int(ids[i]).to_bytes(4, "little"); ptr += 4
        out[ptr:ptr+4] = int(ts[i]).to_bytes(4, "little"); ptr += 4
        out[ptr:ptr+2] = int(levels[i]).to_bytes(2, "little"); ptr += 2
        out[ptr:ptr+2] = int(lens[i]).to_bytes(2, "little"); ptr += 2
    return checksum_bytes(out)


def main() -> None:
    ids, ts, levels, lens = make_input(17000)
    expected = expected_checksum(ids, ts, levels, lens)
    start = time.perf_counter_ns()
    actual = solution.run(ids, ts, levels, lens, 8)
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
