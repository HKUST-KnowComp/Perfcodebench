def run(left: list[int], right: list[int], iters: int) -> int:
    # If no iterations, baseline returns 0
    if iters <= 0:
        return 0

    n = min(len(left), len(right))
    if n == 0:
        return 0

    mask = (1 << 64) - 1
    bc = int.bit_count

    L = left
    R = right

    total = 0
    for i in range(n):
        total += bc((L[i] & R[i]) & mask)

    return total
