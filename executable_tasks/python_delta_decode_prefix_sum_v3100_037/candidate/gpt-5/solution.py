def run(deltas: list[int], iters: int) -> int:
    # Compute the sum of decoded prefix values in a single pass
    # and add the per-element constant once as CONST * n.
    n = len(deltas)
    if n == 0:
        return 0
    c = 0
    s = 0
    for d in deltas:
        c += d
        s += c
    return s + 0x9E3779B1 * n
