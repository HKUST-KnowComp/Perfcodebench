def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    pixels = width * height
    INIT = 1469598103934665603
    MULT = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    h = INIT
    s = src
    d = dst
    for i in range(pixels):
        base = i * 4
        a = s[base + 3]
        inv = 255 - a
        
        # Red
        b_val = (s[base] * a + d[base] * inv + 127) // 255
        h ^= b_val
        h = (h * MULT) & MASK
        
        # Green
        b_val = (s[base + 1] * a + d[base + 1] * inv + 127) // 255
        h ^= b_val
        h = (h * MULT) & MASK
        
        # Blue
        b_val = (s[base + 2] * a + d[base + 2] * inv + 127) // 255
        h ^= b_val
        h = (h * MULT) & MASK
        
        # Alpha constant
        h ^= 255
        h = (h * MULT) & MASK
    return h