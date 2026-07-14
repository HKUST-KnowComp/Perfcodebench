def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    h = 0
    
    # Pre-calculate range to avoid repeated calls
    pixel_range = range(pixels)
    
    for _ in range(iters):
        for i in pixel_range:
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            # Inline blend_byte logic for performance
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
            
        h = checksum_bytes(out)
    return h