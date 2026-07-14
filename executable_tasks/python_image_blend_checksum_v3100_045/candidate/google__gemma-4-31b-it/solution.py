def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for b in data:
        h = ((h ^ b) * mult) & mask
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Pre-calculate indices to avoid repeated multiplication in the loop
    indices = [i * 4 for i in range(pixels)]
    
    h = 0
    for _ in range(iters):
        out = bytearray(pixels * 4)
        for base in indices:
            a = src[base + 3]
            inv_a = 255 - a
            
            # Inlining blend_byte: ((s * a) + (d * (255 - a)) + 127) // 255
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
            
        h = checksum_bytes(out)
    return h