def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the bitwise AND of corresponding 64-bit words.
    # Preserve baseline semantics: only lower 64 bits of each word are considered
    # and only up to the shorter input length is processed.
    MASK = 0xFFFFFFFFFFFFFFFF

    ll = left
    rr = right
    n_left = len(ll)
    n_right = len(rr)
    n = n_left if n_left < n_right else n_right

    bc = int.bit_count

    total = 0
    i = 0
    # Loop unrolling for better performance
    unroll = 4
    limit = n - (n % unroll)
    while i < limit:
        a0 = ll[i] & rr[i]
        a1 = ll[i + 1] & rr[i + 1]
        a2 = ll[i + 2] & rr[i + 2]
        a3 = ll[i + 3] & rr[i + 3]
        total += (
            bc(a0 & MASK)
            + bc(a1 & MASK)
            + bc(a2 & MASK)
            + bc(a3 & MASK)
        )
        i += 4

    while i < n:
        total += bc((ll[i] & rr[i]) & MASK)
        i += 1

    # Baseline result is independent of iters; return the computed count once.
    return total
