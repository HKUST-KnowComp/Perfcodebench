def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Pre-allocate output buffer to avoid repeated allocations and extensions
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Cache constants and functions locally to minimize lookup overhead
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1
    
    # Local references for speed
    src_view = src
    dst_view = dst
    out_view = out

    h = 0
    for _ in range(iters):
        # Optimized blending loop
        # We process 4 bytes (RGBA) at a time
        for i in range(pixels):
            base = i << 2
            a = src_view[base + 3]
            
            if a == 255:
                # Fully opaque source
                out_view[base] = src_view[base]
                out_view[base + 1] = src_view[base + 1]
                out_view[base + 2] = src_view[base + 2]
            elif a == 0:
                # Fully transparent source
                out_view[base] = dst_view[base]
                out_view[base + 1] = dst_view[base + 1]
                out_view[base + 2] = dst_view[base + 2]
            else:
                # Standard blend
                inv_a = 255 - a
                # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
                out_view[base] = (src_view[base] * a + dst_view[base] * inv_a + 127) // 255
                out_view[base + 1] = (src_view[base + 1] * a + dst_view[base + 1] * inv_a + 127) // 255
                out_view[base + 2] = (src_view[base + 2] * a + dst_view[base + 2] * inv_a + 127) // 255
            
            out_view[base + 3] = 255

        # Optimized FNV-1a checksum
        curr_h = FNV_OFFSET
        for b in out_view:
            curr_h = ((curr_h ^ b) * FNV_PRIME) & MASK64
        h = curr_h
        
    return h