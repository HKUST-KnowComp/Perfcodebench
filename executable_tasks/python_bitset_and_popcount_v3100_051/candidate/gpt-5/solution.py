def run(left: list[int], right: list[int], iters: int) -> int:
    m = (1 << 64) - 1
    bc = int.bit_count
    total = 0
    for a, b in zip(left, right):
        total += bc((a & b) & m)
    return total
