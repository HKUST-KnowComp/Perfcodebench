def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Precompute blend table: table[a][s][d] = ((s * a) + (d * (255 - a)) + 127) // 255
    # To save memory, we use the property: result = d + ((a * (s - d) + 127) // 255)
    # But for speed, a flat 256*256 table for (a, diff) is sufficient.
    # Actually, a 64KB table for (a, s, d) is too large, so we use the optimized math.
    
    def checksum_bytes(data: bytes) -> int:
        h = 1469598103934665603
        for b in data:
            h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        return h

    out = bytearray(pixels * 4)
    src_view = memoryview(src)
    dst_view = memoryview(dst)
    
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            out[base] = (src[base] * a + dst[base] * inv_a + 127) // 255
            out[base + 1] = (src[base + 1] * a + dst[base + 1] * inv_a + 127) // 255
            out[base + 2] = (src[base + 2] * a + dst[base + 2] * inv_a + 127) // 255
            out[base + 3] = 255
        h = checksum_bytes(out)
    return h