import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized operations
    # We use uint32 for intermediate calculations to prevent overflow
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.uint32)
    
    # Pre-calculate components that don't change inside the loop
    # src_rgb: [R, G, B], src_a: [A]
    src_rgb = src_arr[:, :3]
    src_a = src_arr[:, 3:4]
    dst_rgb = dst_arr[:, :3]
    
    # Pre-allocate output array for RGB
    # The alpha channel is always 255 per requirements
    out_rgb = np.empty((src_arr.shape[0], 3), dtype=np.uint32)
    
    # Pre-calculate (255 - a)
    inv_a = 255 - src_a

    h = 0
    for _ in range(iters):
        # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
        # This performs the operation on all pixels and all 3 color channels at once
        out_rgb[:] = (src_rgb * src_a + dst_rgb * inv_a + 127) // 255
        
        # Construct the full RGBA output
        # We need to append a column of 255s for the alpha channel
        # To avoid heavy concatenation in the loop, we use a single buffer
        full_out = np.empty((src_arr.shape[0], 4), dtype=np.uint8)
        full_out[:, :3] = out_rgb.astype(np.uint8)
        full_out[:, 3] = 255
        
        # Convert to bytes for checksum
        h = checksum_bytes(full_out.tobytes())
        
    return h