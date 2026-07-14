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
    state = 0x9988221144776655
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        u = (state >> 16) & 0xFFFF
        s = (state >> 28) & 0x7FF
        f = "true" if (i % 3) == 0 else "false"
        rows.append(f'{{"u":{u},"s":{s},"f":{f},"tag":"t{i % 95}"}}')
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
        u_pos = row.index('"u":') + 4
        s_pos = row.index('"s":') + 4
        f = '"f":true' in row
        u = parse_int_at(row, u_pos)
        s = parse_int_at(row, s_pos)
        v = (u & 1023) + s + (17 if f else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h


def main() -> None:
    run = load_run()
    rows = make_rows(267500)
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
