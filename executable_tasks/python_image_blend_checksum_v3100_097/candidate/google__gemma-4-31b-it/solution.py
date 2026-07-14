def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate constants for the blend
    # blend = ((s * a) + (d * (255 - a)) + 127) // 255
    
    # To optimize, we avoid function calls and use local variables
    # We use a memoryview for faster access to the bytearrays
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)
    
    h_final = 0
    
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src_mv[base + 3]
            inv_a = 255 - a
            
            # Unrolled blend calculations
            out_mv[base] = ((src_mv[base] * a) + (dst_mv[base] * inv_a) + 127) // 255
            out_mv[base + 1] = ((src_mv[base + 1] * a) + (dst_mv[base + 1] * inv_a) + 127) // 255
            out_mv[base + 2] = ((src_mv[base + 2] * a) + (dst_mv[base + 2] * inv_a) + 127) // 255
            out_mv[base + 3] = 255
        
        # Inline checksum_bytes for performance
        h = 1469598103934665603
        mask = (1 << 64) - 1
        for b in out:
            h ^= b
            h = (h * 1099511628211) & mask
        h_final = h
        
    return h_final