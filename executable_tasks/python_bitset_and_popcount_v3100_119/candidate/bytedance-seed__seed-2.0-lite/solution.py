def run(left: list[int], right: list[int], iters: int) -> int:
    final_total = 0
    for _ in range(iters):
        final_total = sum((a & b).bit_count() for a, b in zip(left, right))
    return final_total