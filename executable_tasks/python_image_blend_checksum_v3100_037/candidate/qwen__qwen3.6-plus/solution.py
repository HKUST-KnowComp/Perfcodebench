def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    
    total_bytes = pixels * 4
    for i in range(0, total_bytes, 4):
        a = src[i+3]
        inv_a = 255 - a
        
        # R
        val = (src[i] * a + dst[i] * inv_a + 127) // 255
        h ^= val
        h = (h * FNV_PRIME) & MASK
        
        # G
        val = (src[i+1] * a + dst[i+1] * inv_a + 127) // 255
        h ^= val
        h = (h * FNV_PRIME) & MASK
        
        # B
        val = (src[i+2] * a + dst[i+2] * inv_a + 127) // 255
        h ^= val
        h = (h * FNV_PRIME) & MASK
        
        # A
        h ^= 255
        h = (h * FNV_PRIME) & MASK
        
    return h