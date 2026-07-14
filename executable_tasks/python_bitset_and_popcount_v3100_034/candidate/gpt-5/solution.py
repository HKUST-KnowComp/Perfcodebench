def run(left: list[int], right: list[int], iters: int) -> int:
    # Preserve baseline behavior: no iterations => return 0
    if iters <= 0:
        return 0

    # Compute the popcount of the 64-bit-masked AND once; result is identical per iteration
    n_left = len(left)
    n_right = len(right)
    n = n_left if n_left <= n_right else n_right

    mask = (1 << 64) - 1
    bc = int.bit_count
    l = left
    r = right

    total = 0
    for i in range(n):
        total += bc((l[i] & r[i]) & mask)

    return total
