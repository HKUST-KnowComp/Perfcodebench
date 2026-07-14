def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out_size = pixels * 4
    out = bytearray(out_size)
    
    for _ in range(iters):
        # Blend all pixels
        for i in range(pixels):
            base = i * 4
            out_base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Blend RGB channels inline
            out[out_base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[out_base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[out_base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[out_base + 3] = 255
        
        # Compute checksum inline
        h = 1469598103934665603
        for b in out:
            h ^= b
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h
