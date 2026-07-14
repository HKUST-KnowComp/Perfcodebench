def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-cache local references for speed
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    out_set = out.__setitem__

    # Pre-calculate blending logic to avoid function call overhead
    # The formula is: ((s * a) + (d * (255 - a)) + 127) // 255
    
    for _ in range(iters):
        for i in range(pixels):
            base = i << 2
            a = src[base + 3]
            
            # Optimization: if alpha is 255, result is src; if 0, result is dst
            # But we must follow the exact formula provided for correctness.
            inv_a = 255 - a
            
            # Unroll the blend_byte logic
            out[base]     = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255

        # Optimized checksum loop
        h = 1469598103934665603
        mask = (1 << 64) - 1
        for b in out:
            h = ((h ^ b) * 1099511628211) & mask
            
    return h