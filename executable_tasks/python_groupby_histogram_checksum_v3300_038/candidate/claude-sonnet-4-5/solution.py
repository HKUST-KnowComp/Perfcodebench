from array import array

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        hist = array('q', [0] * buckets)
        
        # Direct indexing instead of zip
        n = len(keys)
        i = 0
        
        # Loop unrolling by 4
        while i + 3 < n:
            hist[keys[i]] += vals[i]
            hist[keys[i+1]] += vals[i+1]
            hist[keys[i+2]] += vals[i+2]
            hist[keys[i+3]] += vals[i+3]
            i += 4
        
        # Handle remainder
        while i < n:
            hist[keys[i]] += vals[i]
            i += 1
        
        # FNV-1a hash computation
        h = 1469598103934665603
        j = 0
        
        # Unroll hash computation by 4
        while j + 3 < buckets:
            h ^= hist[j] + 1
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= hist[j+1] + 1
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= hist[j+2] + 1
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            h ^= hist[j+3] + 1
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            j += 4
        
        # Handle remainder
        while j < buckets:
            h ^= hist[j] + 1
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            j += 1
    
    return h