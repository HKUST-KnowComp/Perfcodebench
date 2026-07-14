def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    h = 1469598103934665603
    for idx in ids:
        h = ((h ^ (dict_vals[idx] + 1)) * 1099511628211) & 18446744073709551615
    return h
