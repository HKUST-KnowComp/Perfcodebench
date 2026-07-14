import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 146959810394665603
    # FNV-1a 64-bit implementation
    # Using a loop for the checksum as it is inherently sequential,
    # but we ensure the data is passed efficiently.
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized math
    # We use uint32 for intermediate calculations to prevent overflow
    src_np = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    
    pixels = width * height
    out_np = np.empty((pixels, 4), dtype=np.uint8)
    
    # Pre-extract alpha to avoid repeated indexing
    alpha = src_np[:, 3:4]
    inv_alpha = 255 - alpha
    
    # Perform vectorized blending
    # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We only blend RGB (indices 0, 1, 2)
    rgb_src = src_np[:, :3]
    rgb_dst = dst_np[:, :3]
    
    blended_rgb = ((rgb_src * alpha) + (rgb_dst * inv_alpha) + 127) // 255
    
    # Set Alpha channel to 255 as per original requirement
    out_np[:, :3] = blended_rgb.astype(np.uint8)
    out_np[:, 3] = 255
    
    # Flatten for checksum
    out_bytes = out_np.tobytes()
    
    h = 0
    for _ in range(iters):
        h = checksum_bytes(out_bytes)
        
    return h