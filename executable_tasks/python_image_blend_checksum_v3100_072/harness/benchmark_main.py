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

def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def make_input(pixels: int):
    src = bytearray(pixels * 4)
    dst = bytearray(pixels * 4)
    state = 0x123456789ABCDEF0
    for i in range(pixels):
        base = i * 4
        for c in range(3):
            state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
            src[base + c] = (state >> 56) & 0xFF
            state = (state * 2862933555777941757 + 3037000493) & ((1 << 64) - 1)
            dst[base + c] = (state >> 56) & 0xFF
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        src[base + 3] = (state >> 56) & 0xFF
        dst[base + 3] = 255
    return src, dst


def expected_checksum(src: bytearray, dst: bytearray, width: int, height: int) -> int:
    out = bytearray(len(src))
    for i in range(width * height):
        base = i * 4
        a = src[base + 3]
        out[base] = blend_byte(src[base], dst[base], a)
        out[base + 1] = blend_byte(src[base + 1], dst[base + 1], a)
        out[base + 2] = blend_byte(src[base + 2], dst[base + 2], a)
        out[base + 3] = 255
    return checksum_bytes(out)


def main() -> None:
    width = 160
    height = 260
    iters = 10
    src, dst = make_input(width * height)
    expected = expected_checksum(src, dst, width, height)
    start = time.perf_counter_ns()
    actual = solution.run(src, dst, width, height, iters)
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
