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
    
    # Vectorized blend for RGB channels
    # blend = (src * alpha + dst * (255 - alpha) + 127) // 255
    for c in range(3):
        s = src_arr[:, c].astype(np.uint16)
        d = dst_arr[:, c].astype(np.uint16)
        out[:, c] = ((s * alpha + d * inv_alpha + 127) // 255).astype(np.uint8)
    
    # Alpha channel is always 255
    out[:, 3] = 255
    
    # FNV-1a hash constants
    FNV_PRIME = np.uint64(1099511628211)
    MASK = np.uint64((1 << 64) - 1)
    
    h = np.uint64(0)
    for _ in range(iters):
        # Compute checksum using FNV-1a
        h = np.uint64(1469598103934665603)
        flat = out.ravel()
        for b in flat:
            h ^= np.uint64(b)
            h = (h * FNV_PRIME) & MASK
    
    return int(h)
