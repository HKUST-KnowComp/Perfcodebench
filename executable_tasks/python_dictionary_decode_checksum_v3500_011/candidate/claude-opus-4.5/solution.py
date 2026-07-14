import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    ids_arr = np.asarray(ids, dtype=np.intp)
    vals_arr = np.asarray(dict_vals)
    
    decoded = vals_arr[ids_arr]
    decoded_plus_one = decoded + 1
    decoded_list = decoded_plus_one.tolist()
    
    mask = (1 << 64) - 1
    mult = 1099511628211
    init_h = 1469598103934665603
    
    for _ in range(iters):
        h = init_h
        for v in decoded_list:
            h = ((h ^ v) * mult) & mask
    
    return h
