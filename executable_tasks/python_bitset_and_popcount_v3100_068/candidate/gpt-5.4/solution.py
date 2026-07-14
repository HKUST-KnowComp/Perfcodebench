def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    lzip = zip
    for _ in range(iters):
        s = 0
        for a, b in lzip(left, right):
            s += (a & b).bit_count()
        total = s
    return total
