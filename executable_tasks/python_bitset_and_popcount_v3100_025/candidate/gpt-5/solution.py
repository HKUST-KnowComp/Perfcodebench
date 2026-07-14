def run(left: list[int], right: list[int], iters: int) -> int:
    # Match baseline behavior: if no iterations, return 0
    if iters <= 0:
        return 0

    # Compute popcount of (left[i] & right[i]) over the common length, restricted to 64 bits
    limit = len(left) if len(left) < len(right) else len(right)
    l = left
    r = right
    mask = (1 << 64) - 1
    bc = int.bit_count

    total = 0
    for i in range(limit):
        total += bc((l[i] & r[i]) & mask)

    return total