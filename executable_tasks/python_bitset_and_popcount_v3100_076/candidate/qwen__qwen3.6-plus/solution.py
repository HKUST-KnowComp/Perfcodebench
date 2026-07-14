def run(left: list[int], right: list[int], iters: int) -> int:
    pairs = list(zip(left, right))
    total = 0
    for _ in range(iters):
        total = sum((a & b).bit_count() for a, b in pairs)
    return total