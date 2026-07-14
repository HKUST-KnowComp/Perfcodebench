def run(left: list[int], right: list[int], iters: int) -> int:
    n = len(left)
    total = 0

    left_local = left
    right_local = right
    bit_count = int.bit_count

    for _ in range(iters):
        s = 0
        for i in range(n):
            s += bit_count(left_local[i] & right_local[i])
        total = s

    return total
