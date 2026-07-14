def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    const = 0x9E3779B1
    pref = 0
    total = 0
    for d in deltas:
        pref += d
        total += pref
    total += n * const
    return total