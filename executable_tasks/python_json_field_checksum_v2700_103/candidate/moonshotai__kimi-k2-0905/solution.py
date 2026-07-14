import re

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1
    mul = 1099511628211

    # Pre-compiled regex for faster field extraction
    pattern = re.compile(
        r'"u"\s*:\s*(-?\d+).*?"s"\s*:\s*(-?\d+).*?"f"\s*:\s*(true|false)',
        re.DOTALL
    )

    for row in rows:
        match = pattern.search(row)
        if not match:
            # Fallback to full JSON parse for edge cases
            import json
            obj = json.loads(row)
            u = obj["u"] & 1023
            s = obj["s"]
            f = 17 if obj["f"] else 0
            v = u + s + f
        else:
            # Fast path using regex groups
            u = int(match.group(1)) & 1023
            s = int(match.group(2))
            f = 17 if match.group(3) == "true" else 0
            v = u + s + f

        h ^= (v + 1)
        h *= mul
        h &= mask64

    return h