def run(left: list[int], right: list[int], iters: int) -> int:
    if not iters:
        return 0
    pop = 0
    for a, b in zip(left, right):
        pop += (a & b).bit_count()
    return pop
