def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the 64-bit AND across corresponding words.
    # Only the least-significant 64 bits are considered, matching the baseline's behavior.
    mask = 0xFFFFFFFFFFFFFFFF
    n_left = len(left)
    n_right = len(right)
    limit = n_left if n_left < n_right else n_right

    total = 0
    li = left  # local aliases for speed
    ri = right

    for i in range(limit):
        total += ((li[i] & ri[i]) & mask).bit_count()

    return total
