import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Pre-compute blended RGB channels using uint16 to avoid overflow
    # blend = (s * a + d * (255 - a) + 127) // 255
    src_rgb = src_arr[:, :3].astype(np.uint16)
    dst_rgb = dst_arr[:, :3].astype(np.uint16)
    
    blended_rgb = ((src_rgb * alpha[:, np.newaxis] + dst_rgb * inv_alpha[:, np.newaxis] + 127) // 255).astype(np.uint8)
    
    # Build output array with alpha = 255
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, :3] = blended_rgb
    out[:, 3] = 255
    
    # Pre-compute FNV lookup table for checksum
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    fnv_table = np.empty(256, dtype=np.uint64)
    for b in range(256):
        fnv_table[b] = (b * FNV_PRIME) & MASK
    
    # Get flat view of output for checksum
    out_flat = out.ravel()
    
    h = 0
    for _ in range(iters):
        # Checksum using FNV-1a variant
        h = np.uint64(1469598103934665603)
        for b in out_flat:
            h = fnv_table[int(h) ^ b] ^ (h >> 8)
            h = (h * np.uint64(FNV_PRIME)) & MASK
        # Proper FNV-1a
        h = np.uint64(1469598103934665603)
        for b in out_flat:
            h ^= np.uint64(b)
            h = (h * np.uint64(FNV_PRIME)) & MASK
        h = int(h)
    
    return h
