def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    add_const = 0x9E3779B1
    deltas_local = deltas

    total = 0
    for _ in range(iters):
        current = 0
        acc = 0
        for d in deltas_local:
            current += d
            acc += current
        total = acc + n * add_const
    return total
