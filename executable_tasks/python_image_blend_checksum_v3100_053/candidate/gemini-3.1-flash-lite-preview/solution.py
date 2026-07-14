def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate blending table for alpha 0-255 to avoid division/multiplication in loop
    # blend(s, d, a) = ((s * a) + (d * (255 - a)) + 127) // 255
    # This is equivalent to ((s * a + d * (255 - a) + 127) // 255)
    table = [[((s * a + d * (255 - a) + 127) // 255) for d in range(256)] for a in range(256)]
    
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            t_a = table[a]
            out[base] = t_a[dst[base]] if a == 0 else table[a][dst[base]] # Logic simplified
            # Re-implementing the loop for speed
            out[base] = table[a][dst[base]]
            out[base+1] = table[a][dst[base+1]]
            out[base+2] = table[a][dst[base+2]]
            out[base+3] = 255
        h = checksum_bytes(out)
    return h