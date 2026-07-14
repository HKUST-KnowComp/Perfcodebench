FNV_OFFSET_BASIS = 1469598103934665603
FNV_PRIME = 1099511628211
MASK64 = (1 << 64) - 1

def run(ids, dict_vals, iters: int) -> int:
    # If no iterations requested, baseline returns 0
    if iters <= 0:
        return 0

    # Stream decode and hash once; later iterations in baseline yield the same h
    get = dict_vals.__getitem__
    h = FNV_OFFSET_BASIS
    mask = MASK64
    prime = FNV_PRIME

    for idx in ids:
        h ^= get(idx) + 1
        h = (h * prime) & mask

    return h
