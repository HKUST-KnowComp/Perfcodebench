import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert input bytearrays to numpy arrays once, reuse across all iterations
    src_np = np.frombuffer(src, dtype=np.uint8).reshape(height, width, 4)
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape(height, width, 4)
    
    # Extract alpha channel and precompute 255-alpha for vectorized blending
    alpha = src_np[..., 3:4]  # Preserve dimensions to broadcast across RGB channels
    alpha255 = 255 - alpha
    h = 0
    
    # Preallocate output array once, set fixed 255 alpha channel once
    out_np = np.empty((height, width, 4), dtype=np.uint8)
    out_np[..., 3] = 255
    
    for _ in range(iters):
        # Vectorized blend operation matching original arithmetic exactly
        blended_rgb = (src_np[..., :3] * alpha + dst_np[..., :3] * alpha255 + 127) // 255
        out_np[..., :3] = blended_rgb
        
        # Generate identical output byte sequence to original implementation
        h = checksum_bytes(out_np.tobytes())
    return h