def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate blend table for alpha values 0-255 to avoid repeated math
    # blend(s, d, a) = ((s * a) + (d * (255 - a)) + 127) // 255
    # This is equivalent to ((s - d) * a + d * 255 + 127) // 255
    table = [[((s * a) + (d * (255 - a)) + 127) // 255 for d in range(256)] for s in range(256)]
    
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            t_a = table[src[base]]
            out[base] = t_a[dst[base]]
            t_a = table[src[base + 1]]
            out[base + 1] = t_a[dst[base + 1]]
            t_a = table[src[base + 2]]
            out[base + 2] = t_a[dst[base + 2]]
            out[base + 3] = 255
            
        # Inline checksum calculation for performance
        h = 1469598103934665603
        for b in out:
            h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h