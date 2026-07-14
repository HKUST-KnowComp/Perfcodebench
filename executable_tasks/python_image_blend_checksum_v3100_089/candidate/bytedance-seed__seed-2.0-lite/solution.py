import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert input bytearrays to numpy arrays once for reuse across all iterations
    src_np = np.frombuffer(src, dtype=np.uint8).reshape(height, width, 4)
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape(height, width, 4)
    
    # Pre-allocate output array once, set fixed output alpha (always 255) once
    out_np = np.empty((height, width, 4), dtype=np.uint8)
    out_np[:, :, 3] = 255
    
    # Pre-calculate fixed blending inputs to avoid repeated conversions
    src_rgb = src_np[..., :3].astype(np.int32)
    dst_rgb = dst_np[..., :3].astype(np.int32)
    alpha = src_np[..., 3:4].astype(np.int32)
    inv_alpha = 255 - alpha
    BLEND_OFFSET = np.int32(127)
    SCALE = np.int32(255)
    
    h = 0
    for _ in range(iters):
        # Vectorized blend, identical math to original per-pixel implementation
        blended_rgb = (src_rgb * alpha + dst_rgb * inv_alpha + BLEND_OFFSET) // SCALE
        out_np[..., :3] = blended_rgb.astype(np.uint8)
        h = checksum_bytes(out_np.tobytes())
    return h