def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    h = 0
    
    for iteration in range(iters):
        if iteration == iters - 1:
            # Final iteration: blend and checksum together
            h = 1469598103934665603
            out_idx = 0
            for i in range(pixels):
                base = i * 4
                a = src[base + 3]
                inv_a = 255 - a
                
                b0 = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
                out[out_idx] = b0
                h ^= b0
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                out_idx += 1
                
                b1 = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
                out[out_idx] = b1
                h ^= b1
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                out_idx += 1
                
                b2 = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
                out[out_idx] = b2
                h ^= b2
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                out_idx += 1
                
                out[out_idx] = 255
                h ^= 255
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                out_idx += 1
        else:
            # Non-final iterations: blend only
            out_idx = 0
            for i in range(pixels):
                base = i * 4
                a = src[base + 3]
                inv_a = 255 - a
                
                out[out_idx] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
                out_idx += 1
                out[out_idx] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
                out_idx += 1
                out[out_idx] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
                out_idx += 1
                out[out_idx] = 255
                out_idx += 1
    
    return h
