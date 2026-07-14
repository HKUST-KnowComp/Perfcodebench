def run(left: list[int], right: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    return sum([((a & b) & 0xFFFFFFFFFFFFFFFF).bit_count() for a, b in zip(left, right)])
