import importlib.util
import json
import os
import time


def load_run():
    solution_path = os.environ["PERFCODEBENCH_SOLUTION"]
    spec = importlib.util.spec_from_file_location("perfcodebench_solution", solution_path)
    module = importlib.util.module_from_spec(spec)
    assert spec and spec.loader
    spec.loader.exec_module(module)
    return module.run


def make_rows(n: int) -> list[str]:
    rows = []
    state = 0x8877665544332211
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        u = (state >> 16) & 0xFFFF
        s = (state >> 28) & 0x7FF
        f = 1 if (i % 2) == 0 else 0
        rows.append(f"u={u}&score={s}&flag={f}&k=z{i % 97}")
    return rows


def parse_int_at(s: str, start: int) -> int:
    v = 0
    while start < len(s) and "0" <= s[start] <= "9":
        v = v * 10 + (ord(s[start]) - 48)
        start += 1
    return v


def expected_checksum(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        up = row.index("u=") + 2
        sp = row.index("&score=") + 7
        fp = row.index("&flag=") + 6
        u = parse_int_at(row, up)
        s = parse_int_at(row, sp)
        f = parse_int_at(row, fp)
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h


def main() -> None:
    run = load_run()
    rows = make_rows(373200)
    expected = expected_checksum(rows)
    start = time.perf_counter_ns()
    actual = run(rows)
    elapsed = time.perf_counter_ns() - start
    print(json.dumps({
        "ok": actual == expected,
        "elapsed_ns": elapsed,
        "expected_checksum": expected,
        "actual_checksum": actual,
    }))
    if actual != expected:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
