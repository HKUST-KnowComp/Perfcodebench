def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603
    MASK = 0xFFFFFFFFFFFFFFFF
    h = 0
    
    for _ in range(iters):
        h = FNV_OFFSET
        for i in range(0, pixels * 4, 4):
            a = src[i+3]
            inv_a = 255 - a
            h = ((h ^ ((src[i]*a + dst[i]*inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ ((src[i+1]*a + dst[i+1]*inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ ((src[i+2]*a + dst[i+2]*inv_a + 127) // 255)) * FNV_PRIME) & MASK
            h = ((h ^ 255) * FNV_PRIME) & MASK
    return h