def run(left: list[int], right: list[int], iters: int) -> int:
    n = len(left)
    if n == 0 or iters <= 0:
        return 0

    total = 0
    left_local = left
    right_local = right
    min_len = n if n < len(right_local) else len(right_local)

    for _ in range(iters):
        s = 0
        for i in range(min_len):
            s += (left_local[i] & right_local[i]).bit_count()
        total = s
    return total
