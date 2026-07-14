def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Precompute blend table: table[a][s][d] = ((s * a) + (d * (255 - a)) + 127) // 255
    # To save memory, we use the property: result = d + ((s - d) * a + 127) // 255
    # However, a full 256x256x256 table is too large. 
    # We use the optimized formula: ((s * a) + (d * (255 - a)) + 127) // 255
    
    out = bytearray(pixels * 4)
    out_mv = memoryview(out)
    
    # Precompute blend table for (s, a) -> (s*a + 127)//255
    # This reduces the inner loop to two additions and a shift/division
    # Actually, the most efficient way in Python is to minimize attribute lookups
    
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
        
        h = checksum_bytes(out)
    return h