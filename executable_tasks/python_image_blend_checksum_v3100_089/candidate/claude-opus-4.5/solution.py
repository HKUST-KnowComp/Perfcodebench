import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel and compute inverse alpha
    alpha = src_arr[:, 3].astype(np.uint32)
    inv_alpha = 255 - alpha
    
    # Pre-compute blended RGB channels using vectorized operations
    # blend = (s * a + d * (255 - a) + 127) // 255
    src_rgb = src_arr[:, :3].astype(np.uint32)
    dst_rgb = dst_arr[:, :3].astype(np.uint32)
    
    blended_rgb = (src_rgb * alpha[:, np.newaxis] + dst_rgb * inv_alpha[:, np.newaxis] + 127) // 255
    blended_rgb = blended_rgb.astype(np.uint8)
    
    # Create output array with alpha = 255
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, :3] = blended_rgb
    out[:, 3] = 255
    
    # Pre-build FNV-1a lookup table for speed
    MASK = (1 << 64) - 1
    FNV_PRIME = 1099511628211
    
    # Build table: table[b] = (b * FNV_PRIME) & MASK for XOR-then-multiply pattern
    # Actually FNV-1a is: h ^= b; h *= prime
    # We can't fully table this, but we can speed up the loop
    
    h = 0
    out_flat = out.ravel()
    out_bytes = out_flat.tobytes()
    
    for _ in range(iters):
        # FNV-1a hash
        h = 1469598103934665603
        for b in out_bytes:
            h ^= b
            h = (h * FNV_PRIME) & MASK
    
    return h
