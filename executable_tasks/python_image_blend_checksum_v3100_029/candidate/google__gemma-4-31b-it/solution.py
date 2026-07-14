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
    
    # Precompute blend tables for all possible alpha values (0-255)
    # table[alpha][src_val] = ((src_val * alpha) + (dst_val * (255 - alpha)) + 127) // 255
    # However, since dst_val changes per pixel, we can't precompute the full result.
    # But we can optimize the math: (s*a + d*(255-a) + 127) // 255
    
    # To maximize speed in pure Python, we avoid function calls and use local variables.
    # We use a pre-allocated bytearray and slice assignment or index access.
    
    h = 0
    # Local references for speed
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    out_set = out.__setitem__
    
    for _ in range(iters):
        idx = 0
        for i in range(pixels):
            base = i * 4
            a = src_get(base + 3)
            inv_a = 255 - a
            
            # Manual inlining of blend_byte
            # Red
            out_set(idx, ((src_get(base) * a) + (dst_get(base) * inv_a) + 127) // 255)
            # Green
            out_set(idx + 1, ((src_get(base + 1) * a) + (dst_get(base + 1) * inv_a) + 127) // 255)
            # Blue
            out_set(idx + 2, ((src_get(base + 2) * a) + (dst_get(base + 2) * inv_a) + 127) // 255)
            # Alpha
            out_set(idx + 3, 255)
            idx += 4
            
        h = checksum_bytes(out)
    return h