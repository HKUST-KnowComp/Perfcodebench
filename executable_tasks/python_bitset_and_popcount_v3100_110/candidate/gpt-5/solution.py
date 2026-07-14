def run(left: list[int], right: list[int], iters: int) -> int:
    # Match baseline semantics: if no iterations, return 0
    if iters <= 0:
        return 0

    n = len(left)
    if len(right) < n:
        n = len(right)

    L = left
    R = right
    mask = 0xFFFFFFFFFFFFFFFF

    total = 0
    for i in range(n):
        total += ((L[i] & R[i]) & mask).bit_count()

    return total
