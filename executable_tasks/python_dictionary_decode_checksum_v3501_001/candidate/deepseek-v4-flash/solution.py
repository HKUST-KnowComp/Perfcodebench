def run(ids, dict_vals, iters: int) -> int:
    # Decode all ids once (list_dictionary lookup)
    decoded = [dict_vals[idx] for idx in ids]
    h = 1469598103934665603
    for v in decoded:
        h ^= v + 1
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h