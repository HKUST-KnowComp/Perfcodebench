import struct

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)
    
    # Pre-calculate blending to avoid function calls and repeated arithmetic
    # Using a flat loop with local variable access
    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            inv_a = 255 - a
            
            out[base] = ((src[base] * a) + (dst[base] * inv_a) + 127) // 255
            out[base + 1] = ((src[base + 1] * a) + (dst[base + 1] * inv_a) + 127) // 255
            out[base + 2] = ((src[base + 2] * a) + (dst[base + 2] * inv_a) + 127) // 255
            out[base + 3] = 255
            
        # FNV-1a checksum implementation
        h = 1469598103934665603
        for b in out:
            h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h