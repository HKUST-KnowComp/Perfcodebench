import re

def run(rows: list[str]) -> int:
    # Pre-compile regex patterns for the specific fields to avoid overhead in the loop
    # We look for "u": <int>, "s": <int>, "f": <bool>
    # Using non-capturing groups and specific patterns to extract values quickly
    u_re = re.compile(r'"u"\s*:\s*(\d+)')
    s_re = re.compile(r'"s"\s*:\s*(\d+)')
    f_re = re.compile(r'"f"\s*:\s*(true|false)')

    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        # Extracting values via regex is often faster than full json.loads for large strings
        # when only a few specific fields are needed.
        u_match = u_re.search(row)
        s_match = s_re.search(row)
        f_match = f_re.search(row)

        u_val = int(u_match.group(1)) if u_match else 0
        s_val = int(s_match.group(1)) if s_match else 0
        f_val = f_match.group(1) == 'true' if f_match else False

        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= v + 1
        h = (h * 1099511628211) & mask64

    return h