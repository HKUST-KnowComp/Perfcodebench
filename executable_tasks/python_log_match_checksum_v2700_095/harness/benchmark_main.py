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
    state = 0x33445566778899AA
    for i in range(n):
        state = (state * 6364136223846793005 + 1442695040888963407) & ((1 << 64) - 1)
        a = chr(ord("A") + ((state >> 60) % 26))
        b = chr(ord("A") + ((state >> 56) % 26))
        x = (state >> 16) % 10000
        if (i % 7) == 0:
            rows.append(f"{a}{b}{x:04d}-{state & 0xFFFFFFFF:08X}")
        else:
            rows.append(f"bad:{state & 0xFFFFFFFF:08X}")
    return rows


def is_hex(c: str) -> bool:
    return ("0" <= c <= "9") or ("A" <= c <= "F")


def matches(s: str) -> bool:
    if len(s) != 15:
        return False
    if not ("A" <= s[0] <= "Z" and "A" <= s[1] <= "Z"):
        return False
    if not all("0" <= s[i] <= "9" for i in range(2, 6)):
        return False
    if s[6] != "-":
        return False
    return all(is_hex(s[i]) for i in range(7, 15))


def expected_checksum(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        v = (ord(row[2]) + ord(row[7])) if matches(row) else 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h


def main() -> None:
    run = load_run()
    rows = make_rows(407000)
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
