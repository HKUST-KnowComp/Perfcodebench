from array import array

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    keys_len = len(keys)
    
    for _ in range(iters):
        hist = array('q', [0] * buckets)
        
        # Direct indexing without zip, unrolled by 4
        i = 0
        while i + 3 < keys_len:
            hist[keys[i]] += vals[i]
            hist[keys[i+1]] += vals[i+1]
            hist[keys[i+2]] += vals[i+2]
            hist[keys[i+3]] += vals[i+3]
            i += 4
        
        # Handle remainder
        while i < keys_len:
            hist[keys[i]] += vals[i]
            i += 1
        
        # Checksum computation with local caching
        h = 1469598103934665603
        mask = (1 << 64) - 1
        mult = 1099511628211
        
        for v in hist:
            h ^= v + 1
            h = (h * mult) & mask
    
    return h