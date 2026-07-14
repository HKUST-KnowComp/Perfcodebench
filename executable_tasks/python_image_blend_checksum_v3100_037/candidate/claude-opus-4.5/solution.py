import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Prepare output array
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, 3] = 255  # Alpha channel is always 255
    
    # Vectorized blend for RGB channels
    # blend = (src * alpha + dst * (255 - alpha) + 127) // 255
    for c in range(3):
        src_c = src_arr[:, c].astype(np.uint16)
        dst_c = dst_arr[:, c].astype(np.uint16)
        blended = (src_c * alpha + dst_c * inv_alpha + 127) // 255
        out[:, c] = blended.astype(np.uint8)
    
    # FNV-1a checksum constants
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        # Flatten output to bytes
        data = out.tobytes()
        for b in data:
            h ^= b
            h = (h * FNV_PRIME) & MASK
    
    return h
