import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert to numpy arrays once
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4)
    
    # Extract alpha channel and broadcast for RGB channels
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Preallocate output array
    out = np.empty_like(src_arr)
    out[:, 3] = 255  # Alpha channel is always 255
    
    # Vectorized blend for RGB channels
    # blend = (src * alpha + dst * (255 - alpha) + 127) // 255
    for c in range(3):
        s = src_arr[:, c].astype(np.uint16)
        d = dst_arr[:, c].astype(np.uint16)
        out[:, c] = ((s * alpha + d * inv_alpha + 127) // 255).astype(np.uint8)
    
    # Run iterations (blend result is same each time, just need final checksum)
    # But we must compute blend each iteration per the original logic
    h = 0
    for _ in range(iters):
        # Recompute blend (same result but matches original behavior)
        for c in range(3):
            s = src_arr[:, c].astype(np.uint16)
            d = dst_arr[:, c].astype(np.uint16)
            out[:, c] = ((s * alpha + d * inv_alpha + 127) // 255).astype(np.uint8)
        
        # FNV-1a checksum
        h = 1469598103934665603
        mask = (1 << 64) - 1
        data = out.tobytes()
        for b in data:
            h ^= b
            h = (h * 1099511628211) & mask
    
    return h
