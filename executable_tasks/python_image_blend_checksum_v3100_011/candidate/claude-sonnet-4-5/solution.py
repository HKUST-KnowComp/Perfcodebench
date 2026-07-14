def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out_size = pixels * 4
    out = bytearray(out_size)
    h = 0
    
    for _ in range(iters):
        out_idx = 0
        src_idx = 0
        
        for i in range(pixels):
            a = src[src_idx + 3]
            inv_a = 255 - a
            
            # Blend R
            out[out_idx] = ((src[src_idx] * a) + (dst[src_idx] * inv_a) + 127) // 255
            out_idx += 1
            src_idx += 1
            
            # Blend G
            out[out_idx] = ((src[src_idx] * a) + (dst[src_idx] * inv_a) + 127) // 255
            out_idx += 1
            src_idx += 1
            
            # Blend B
            out[out_idx] = ((src[src_idx] * a) + (dst[src_idx] * inv_a) + 127) // 255
            out_idx += 1
            src_idx += 1
            
            # Alpha always 255
            out[out_idx] = 255
            out_idx += 1
            src_idx += 1
        
        # Inline checksum
        h = 1469598103934665603
        for b in out:
            h ^= b
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h
