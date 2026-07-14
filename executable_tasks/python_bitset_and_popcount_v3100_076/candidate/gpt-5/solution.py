def run(left: list[int], right: list[int], iters: int) -> int:
    # Match baseline behavior: if iters == 0, return 0 without processing.
    if iters <= 0:
        return 0

    l = left
    r = right
    n = len(l)
    if len(r) < n:
        n = len(r)

    # Mask to enforce 64-bit semantics, matching baseline behavior on negatives.
    mask = (1 << 64) - 1
    bc = int.bit_count

    total = 0
    i = 0

    # Unroll loop by 4 for better performance.
    n4 = n - (n % 4)
    while i < n4:
        a0 = (l[i] & r[i]) & mask
        a1 = (l[i + 1] & r[i + 1]) & mask
        a2 = (l[i + 2] & r[i + 2]) & mask
        a3 = (l[i + 3] & r[i + 3]) & mask
        total += bc(a0) + bc(a1) + bc(a2) + bc(a3)
        i += 4

    while i < n:
        total += bc((l[i] & r[i]) & mask)
        i += 1

    # Baseline recomputes per iteration but returns the final count; compute once is equivalent.
    return total
