def run(left: list[int], right: list[int], iters: int) -> int:
    n = len(left)
    if n == 0 or iters <= 0:
        return 0

    # Local bindings for faster access in tight loops.
    l = left
    r = right
    total = 0

    # Count bits directly from each ANDed machine word.
    # int.bit_count() is implemented in C and is much faster than
    # materializing per-bit arrays or looping over 64 bit positions.
    for _ in range(iters):
        s = 0
        for i in range(n):
            s += (l[i] & r[i]).bit_count()
        total = s
    return total
