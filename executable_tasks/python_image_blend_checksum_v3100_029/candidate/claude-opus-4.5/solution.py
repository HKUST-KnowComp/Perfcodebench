import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Precompute blended RGB channels using vectorized operations
    # Use uint16 to avoid overflow: max value is 255*255 + 255*255 + 127 = 130432
    src_rgb = src_arr[:, :3].astype(np.uint16)
    dst_rgb = dst_arr[:, :3].astype(np.uint16)
    
    # Blend: ((s * a) + (d * (255 - a)) + 127) // 255
    blended_rgb = ((src_rgb * alpha[:, np.newaxis]) + 
                   (dst_rgb * inv_alpha[:, np.newaxis]) + 127) // 255
    blended_rgb = blended_rgb.astype(np.uint8)
    
    # Create output array with alpha = 255
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, :3] = blended_rgb
    out[:, 3] = 255
    out_flat = out.ravel()
    out_bytes = out_flat.tobytes()
    
    # FNV-1a constants
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603
    MASK = (1 << 64) - 1
    
    # Compute checksum
    def checksum_bytes_fast(data: bytes) -> int:
        h = FNV_OFFSET
        for b in data:
            h ^= b
            h = (h * FNV_PRIME) & MASK
        return h
    
    h = 0
    for _ in range(iters):
        h = checksum_bytes_fast(out_bytes)
    
    return h
