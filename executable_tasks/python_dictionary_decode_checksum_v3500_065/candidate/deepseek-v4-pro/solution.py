MASK = (1 << 64) - 1
MUL = 1099511628211
INIT_H = 1469598103934665603

def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    h = INIT_H
    for idx in ids:
        v = dict_vals[idx]
        h ^= v + 1
        h = (h * MUL) & MASK
    return h
