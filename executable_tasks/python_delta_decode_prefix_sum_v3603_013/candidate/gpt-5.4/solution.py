def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    add_const = 0x9E3779B1
    total = 0

    # Fuse delta decoding and aggregation to avoid the intermediate list
    # and the second pass. Semantics match the baseline because total is
    # reset each iteration and only the final iteration's value is returned.
    for _ in range(iters):
        current = 0
        acc = 0
        for d in deltas:
            current += d
            acc += current
        total = acc + n * add_const

    return total
