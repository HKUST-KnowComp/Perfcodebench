def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        # re-use the same accumulator each iteration
        acc = 0
        for a, b in zip(left, right):
            acc += (a & b).bit_count()
        total = acc
    return total