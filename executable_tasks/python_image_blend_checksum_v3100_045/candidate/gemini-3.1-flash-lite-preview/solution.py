import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    s_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    d_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    alpha = s_arr[:, 3:4]
    inv_alpha = 255 - alpha
    
    # Precompute blended RGB channels
    # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
    blended = ((s_arr[:, :3] * alpha) + (d_arr[:, :3] * inv_alpha) + 127) // 255
    
    # Create the output array structure
    out_arr = np.empty((width * height, 4), dtype=np.uint8)
    out_arr[:, :3] = blended
    out_arr[:, 3] = 255
    
    out_bytes = out_arr.tobytes()
    
    # FNV-1a checksum implementation
    h = 1469598103934665603
    for b in out_bytes:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h