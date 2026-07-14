def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    n = len(left)
    bitcount = int.bit_count
    for _ in range(iters):
        s = 0
        for i in range(n):
            s += bitcount(left[i] & right[i])
        total = s
    return total
