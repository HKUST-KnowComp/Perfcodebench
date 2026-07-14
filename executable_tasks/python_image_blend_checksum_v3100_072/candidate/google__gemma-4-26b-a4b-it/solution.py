import numpy as np

def checksum_bytes(data: np.ndarray) -> int:
    h = 146959810394665603
    # The original checksum used a 64-bit FNV-1a style hash.
    # To maintain exact correctness with the original loop-based checksum,
    # we must process the bytes in order.
    # However, since the task requires a full replacement and the original
    # checksum loop is the bottleneck, we use a fast approach if possible,
    # but for exactness, we must ensure the logic matches.
    # The original code: h = (h * 1099511628211) & ((1 << 64) - 1)
    # We'll use a loop but optimize the data access.
    h = 1469598103934665603
    for b in data.flat:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized operations
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    # Pre-calculate components to avoid repeated work in the loop
    # We only need to blend RGB; A is always 255
    src_rgb = src_arr[:, :3]
    dst_rgb = dst_arr[:, :3]
    alpha = src_arr[:, 3:4]
    
    # Pre-allocate output array
    # The output is (pixels, 4). The last channel is always 255.
    out_arr = np.empty((src_arr.shape[0], 4), dtype=np.uint8)
    out_arr[:, 3] = 255

    last_h = 0
    for _ in range(iters):
        # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
        # Using int32 to prevent overflow during intermediate multiplication
        blended_rgb = ((src_rgb * alpha) + (dst_rgb * (255 - alpha)) + 127) // 255
        out_arr[:, :3] = blended_rgb.astype(np.uint8)
        
        # The checksum must be calculated on the byte representation of the output
        # We use the flattened view of the uint8 array
        last_h = checksum_bytes(out_arr)
        
    return last_h