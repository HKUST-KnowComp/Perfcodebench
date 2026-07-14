def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out_size = pixels * 4
    out = bytearray(out_size)
    
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        out_idx = 0
        src_idx = 0
        
        for i in range(pixels):
            a = src[src_idx + 3]
            inv_a = 255 - a
            
            # Blend R
            val = ((src[src_idx] * a) + (dst[src_idx] * inv_a) + 127) // 255
            out[out_idx] = val
            h ^= val
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
            # Blend G
            val = ((src[src_idx + 1] * a) + (dst[src_idx + 1] * inv_a) + 127) // 255
            out[out_idx + 1] = val
            h ^= val
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
            # Blend B
            val = ((src[src_idx + 2] * a) + (dst[src_idx + 2] * inv_a) + 127) // 255
            out[out_idx + 2] = val
            h ^= val
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
            # Alpha = 255
            out[out_idx + 3] = 255
            h ^= 255
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
            src_idx += 4
            out_idx += 4
    
    return h
