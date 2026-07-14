def run(left: list[int], right: list[int], iters: int) -> int:
    n = len(left)
    if n == 0 or iters <= 0:
        return 0

    left_local = left
    right_local = right
    bit_count = int.bit_count
    total = 0

    for _ in range(iters):
        s = 0
        for i in range(n):
            s += bit_count(left_local[i] & right_local[i])
        total = s

    return total
