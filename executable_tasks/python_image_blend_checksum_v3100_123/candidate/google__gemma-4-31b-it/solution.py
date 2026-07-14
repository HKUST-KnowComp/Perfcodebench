def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for b in data:
        h = ((h ^ b) * 1099511628211) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Cache references to speed up loop access
    src_get = src.__getitem__
    dst_get = dst.__getitem__
    out_set = out.__setitem__
    
    h = 0
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src_get(base + 3)
            inv_a = 255 - a
            
            # Inline blend_byte: ((s * a) + (d * (255 - a)) + 127) // 255
            # Process R, G, B
            out_set(base, (src_get(base) * a + dst_get(base) * inv_a + 127) // 255)
            out_set(base + 1, (src_get(base + 1) * a + dst_get(base + 1) * inv_a + 127) // 255)
            out_set(base + 2, (src_get(base + 2) * a + dst_get(base + 2) * inv_a + 127) // 255)
            out_set(base + 3, 255)
            
        h = checksum_bytes(out)
    return h