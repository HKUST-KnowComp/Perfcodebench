def run(ids, dict_vals, iters: int) -> int:
    if not ids:
        return 1469598103934665603
    
    # The original code performs the same lookup and same hash calculation 'iters' times.
    # Since 'ids' and 'dict_vals' do not change, 'decoded' is always the same.
    # The hash 'h' is reset to the offset basis at the start of every iteration.
    # Therefore, the final 'h' is simply the hash of the 'decoded' list.
    
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in decoded:
        h ^= (v + 1)
        h = (h * 1099511628211) & mask
        
    return h