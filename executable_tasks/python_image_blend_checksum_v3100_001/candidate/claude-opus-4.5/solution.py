import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract channels
    src_rgb = src_arr[:, :3].astype(np.uint32)
    dst_rgb = dst_arr[:, :3].astype(np.uint32)
    alpha = src_arr[:, 3].astype(np.uint32)
    
    # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
    alpha_exp = alpha[:, np.newaxis]  # Shape (pixels, 1) for broadcasting
    blended_rgb = ((src_rgb * alpha_exp) + (dst_rgb * (255 - alpha_exp)) + 127) // 255
    
    # Build output array
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, :3] = blended_rgb.astype(np.uint8)
    out[:, 3] = 255
    
    # Get flat bytes view
    out_flat = out.ravel()
    
    # FNV-1a constants
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        # Compute checksum using optimized loop
        h = FNV_OFFSET
        for b in out_flat:
            h = ((h ^ int(b)) * FNV_PRIME) & MASK
    
    return h
