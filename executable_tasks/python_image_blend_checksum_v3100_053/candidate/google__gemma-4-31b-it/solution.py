def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h ^= b
        h = (h * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate blending table for a single channel
    # table[alpha][source_val] = (source_val * alpha + dest_val * (255 - alpha) + 127) // 255
    # However, since dest_val varies, we can't fully table it. 
    # But we can optimize the math: (s*a + d*(255-a) + 127) // 255
    
    # Localize variables for speed
    src_view = memoryview(src)
    dst_view = memoryview(dst)
    out_view = memoryview(out)
    
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src_view[base + 3]
            inv_a = 255 - a
            
            # Unrolling the blend for R, G, B
            # Using the formula: ((s * a) + (d * inv_a) + 127) // 255
            out_view[base] = (src_view[base] * a + dst_view[base] * inv_a + 127) // 255
            out_view[base + 1] = (src_view[base + 1] * a + dst_view[base + 1] * inv_a + 127) // 255
            out_view[base + 2] = (src_view[base + 2] * a + dst_view[base + 2] * inv_a + 127) // 255
            out_view[base + 3] = 255
            
        h = checksum_bytes(out)
    return h