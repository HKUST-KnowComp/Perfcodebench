def run(ids, dict_vals, iters: int) -> int:
    # Local bindings reduce global/name lookup overhead in tight loops.
    ids_local = ids
    dict_local = dict_vals
    n = len(ids_local)

    # FNV-1a 64-bit constants
    h_init = 1469598103934665603
    mul = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    # Fast path for empty input: baseline resets hash each iteration.
    if n == 0:
        return h_init

    h = h_init
    for _ in range(iters):
        h = h_init
        # Fuse decode and checksum to avoid materializing/writing a decoded list.
        for i in range(n):
            h ^= dict_local[ids_local[i]] + 1
            h = (h * mul) & mask
    return h
