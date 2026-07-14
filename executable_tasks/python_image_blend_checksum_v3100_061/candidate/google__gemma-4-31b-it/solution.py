def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Precompute the blend table for all possible s, d, a is too large (256^3).
    # But we can optimize the blend_byte logic.
    # The formula is: ((s * a) + (d * (255 - a)) + 127) // 255
    
    # To optimize, we use a local reference for the checksum loop and avoid function calls.
    # We also use a precomputed table for the common operation if possible, 
    # but since a changes, we'll optimize the loop structure.
    
    # Pre-calculate constants
    MASK64 = (1 << 64) - 1
    PRIME = 1099511628211
    
    # Cache references to speed up access
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    
    h = 0
    for _ in range(iters):
        out = bytearray(pixels * 4)
        out_idx = 0
        for i in range(pixels):
            base = i * 4
            a = src_get(base + 3)
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            # Red
            s = src_get(base)
            d = dst_get(base)
            out[out_idx] = (s * a + d * inv_a + 127) // 255
            # Green
            s = src_get(base + 1)
            d = dst_get(base + 1)
            out[out_idx + 1] = (s * a + d * inv_a + 127) // 255
            # Blue
            s = src_get(base + 2)
            d = dst_get(base + 2)
            out[out_idx + 2] = (s * a + d * inv_a + 127) // 255
            # Alpha
            out[out_idx + 3] = 255
            out_idx += 4
            
        # Optimized checksum_bytes
        curr_h = 1469598103934665603
        for b in out:
            curr_h = ((curr_h ^ b) * PRIME) & MASK64
        h = curr_h
        
    return h