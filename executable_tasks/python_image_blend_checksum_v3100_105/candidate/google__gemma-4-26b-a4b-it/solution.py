import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized operations
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    # Pre-calculate components to avoid repeated work in the loop
    # We only need to blend RGB; A is always 255
    s_rgb = src_arr[:, :3]
    d_rgb = dst_arr[:, :3]
    a = src_arr[:, 3:4]
    
    # Pre-allocate output array
    pixels = width * height
    out_rgb = np.empty((pixels, 3), dtype=np.int32)
    
    # The loop over iters is required by the task signature
    # However, the pixel blending is now vectorized
    last_h = 0
    for _ in range(iters):
        # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
        # Using int32 to prevent overflow during intermediate multiplication
        res_rgb = ((s_rgb * a) + (d_rgb * (255 - a)) + 127) // 255
        
        # Construct the full RGBA output
        # We need to create a uint8 array for the checksum
        # The alpha channel is always 255
        out_rgba = np.empty((pixels, 4), dtype=np.uint8)
        out_rgba[:, :3] = res_rgb.astype(np.uint8)
        out_rgba[:, 3] = 255
        
        last_h = checksum_bytes(out_rgba.tobytes())
        
    return last_h