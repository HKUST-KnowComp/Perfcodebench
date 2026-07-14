def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        # mimic original NameError when iters=0
        raise UnboundLocalError("local variable 'h' referenced before assignment")
    d = dict_vals
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    for idx in ids:
        h ^= d[idx] + 1
        h = (h * mult) & mask
    return h
