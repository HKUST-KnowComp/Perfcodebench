def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    n = len(left)
    for _ in range(iters):
        s = 0
        for i in range(n):
            s += (left[i] & right[i]).bit_count()
        total = s
    return total
