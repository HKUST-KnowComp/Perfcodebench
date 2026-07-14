import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    src_np = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    # Pre-calculate alpha and the (255 - a) term
    a = src_np[:, 3:4]
    inv_a = 255 - a
    
    # Pre-calculate the blended RGB channels
    # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We perform this on the first 3 channels
    blended_rgb = ((src_np[:, :3] * a) + (dst_np[:, :3] * inv_a) + 127) // 255
    
    # Create the full RGBA array (N, 4)
    # The alpha channel is always 255
    out_np = np.empty_like(src_np)
    out_np[:, :3] = blended_rgb.astype(np.uint8)
    out_np[:, 3] = 255
    
    # Flatten to bytes for checksum
    out_bytes = out_np.tobytes()
    
    h = 0
    for _ in range(iters):
        h = checksum_bytes(out_bytes)
    return h