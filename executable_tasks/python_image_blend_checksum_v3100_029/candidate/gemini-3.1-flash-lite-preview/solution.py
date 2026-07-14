def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Precompute blend table: blend[a][s][d] = ((s * a) + (d * (255 - a)) + 127) // 255
    # To save memory, we use the property: blend = (s * a + d * (255 - a) + 127) // 255
    # This is equivalent to: d + ((s - d) * a + 127) // 255
    table = [[((s * a + d * (255 - a) + 127) // 255) for d in range(256)] for a in range(256) for s in range(256)]
    
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate indices to avoid repeated math in the loop
    indices = range(0, pixels * 4, 4)
    
    h = 0
    for _ in range(iters):
        for i in indices:
            a = src[i + 3]
            # Offset into the flattened table: (a * 256 + s) * 256 + d
            a_offset = a * 65536
            out[i] = table[a_offset + src[i] * 256 + dst[i]]
            out[i+1] = table[a_offset + src[i+1] * 256 + dst[i+1]]
            out[i+2] = table[a_offset + src[i+2] * 256 + dst[i+2]]
            out[i+3] = 255
            
        # FNV-1a checksum
        h = 1469598103934665603
        for b in out:
            h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h