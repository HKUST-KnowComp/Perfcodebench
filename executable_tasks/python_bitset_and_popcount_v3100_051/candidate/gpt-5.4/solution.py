def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    bit_count = int.bit_count
    n = len(left)

    for _ in range(iters):
        s = 0
        for i in range(n):
            s += bit_count(left[i] & right[i])
        total = s

    return total
