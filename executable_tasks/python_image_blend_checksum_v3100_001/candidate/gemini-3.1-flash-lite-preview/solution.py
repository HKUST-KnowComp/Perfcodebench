def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    h = 0
    
    # Pre-calculate blending table for alpha 0-255 to avoid division/multiplication in loop
    # blend(s, d, a) = ((s * a) + (d * (255 - a)) + 127) // 255
    # This is equivalent to ((s * a) + (d * (255 - a)) + 127) >> 8 if using fast approximation,
    # but we must match the exact integer division logic.
    table = [[((s * a) + (d * (255 - a)) + 127) // 255 for d in range(256)] for a in range(256)]

    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            t_a = table[a]
            out[base] = t_a[dst[base]] if src[base] == 0 else table[a][dst[base]] # Placeholder logic
            # Re-implementing the loop efficiently
            out[base] = ((src[base] * a) + (dst[base] * (255 - a)) + 127) // 255
            out[base+1] = ((src[base+1] * a) + (dst[base+1] * (255 - a)) + 127) // 255
            out[base+2] = ((src[base+2] * a) + (dst[base+2] * (255 - a)) + 127) // 255
            out[base+3] = 255
            
        h = 1469598103934665603
        for b in out:
            h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h