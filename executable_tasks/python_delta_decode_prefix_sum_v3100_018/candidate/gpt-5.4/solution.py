def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    add_const = 0x9E3779B1

    # Fuse delta decoding and aggregation to avoid the intermediate list
    # and the second pass. Keep local bindings for faster access.
    total = 0
    for _ in range(iters):
        current = 0
        s = 0
        for d in deltas:
            current += d
            s += current
        total = s + n * add_const
    return total
