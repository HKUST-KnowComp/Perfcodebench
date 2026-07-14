def run(left: list[int], right: list[int], iters: int) -> int:
    # If no iterations requested, baseline returns 0; keep that behavior.
    if iters <= 0:
        return 0

    n = min(len(left), len(right))
    mask = (1 << 64) - 1
    l = left
    r = right
    bc = int.bit_count

    total = 0
    for i in range(n):
        total += bc((l[i] & r[i]) & mask)
    return total
