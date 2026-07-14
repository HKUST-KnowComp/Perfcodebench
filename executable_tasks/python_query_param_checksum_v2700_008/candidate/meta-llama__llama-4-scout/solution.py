from urllib.parse import parse_qs

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    U, S, F = "u", "score", "flag"
    for row in rows:
        q = parse_qs(row, keep_blank_values=False, strict_parsing=False)
        u, s, f = int(q[U][0]), int(q[S][0]), int(q[F][0])
        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h