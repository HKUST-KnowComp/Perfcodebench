def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Cache references to local variables for speed
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    out_set = out.__setitem__
    
    # Pre-calculate constants
    MASK64 = (1 << 64) - 1
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603

    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src_get(base + 3)
            inv_a = 255 - a
            
            # Inline blend_byte: ((s * a) + (d * (255 - a)) + 127) // 255
            out_set(base, (src_get(base) * a + dst_get(base) * inv_a + 127) // 255)
            out_set(base + 1, (src_get(base + 1) * a + dst_get(base + 1) * inv_a + 127) // 255)
            out_set(base + 2, (src_get(base + 2) * a + dst_get(base + 2) * inv_a + 127) // 255)
            out_set(base + 3, 255)
        
        # Optimized checksum_bytes
        curr_h = FNV_OFFSET
        for b in out:
            curr_h ^= b
            curr_h = (curr_h * FNV_PRIME) & MASK64
        h = curr_h
        
    return h