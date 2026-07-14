def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Pre-calculate indices to avoid repeated multiplication
    indices = [i * 4 for i in range(pixels)]
    h = 0
    
    for _ in range(iters):
        out = bytearray(pixels * 4)
        for i in range(pixels):
            base = indices[i]
            a = src[base + 3]
            inv_a = 255 - a
            
            # Inline blend_byte logic: ((s * a) + (d * (255 - a)) + 127) // 255
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
            
        h = checksum_bytes(out)
    return h