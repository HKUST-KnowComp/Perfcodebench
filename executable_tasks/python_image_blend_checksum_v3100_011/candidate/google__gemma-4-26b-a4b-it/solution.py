import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    src_np = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    # Pre-calculate components to avoid repeated work in the loop
    # src_rgb: all channels except alpha
    # dst_rgb: all channels except alpha
    # a: alpha channel
    src_rgb = src_np[:, :3]
    dst_rgb = dst_np[:, :3]
    a = src_np[:, 3:4]
    
    # Pre-calculate (255 - a)
    inv_a = 255 - a
    
    # Pre-allocate output array for the RGB part
    # We use int32 to prevent overflow during intermediate multiplication
    res_rgb = np.empty((width * height, 3), dtype=np.int32)
    
    # The alpha channel of the output is always 255
    # We'll construct the full RGBA array for checksumming
    # To save memory/time, we can handle the checksum on the flattened result
    
    h_const = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    last_h = 0
    
    for _ in range(iters):
        # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
        res_rgb[:] = ((src_rgb * a) + (dst_rgb * inv_a) + 127) // 255
        
        # Construct the full RGBA byte sequence for checksum
        # We need to interleave the 255 alpha channel
        # Instead of creating a massive array, we can compute checksum more efficiently
        # but for correctness, let's build the bytearray
        
        # Create the full RGBA array: [R, G, B, 255, R, G, B, 255, ...]
        # We can use a trick: create an array of 255s and overwrite RGB
        full_rgba = np.full((width * height, 4), 255, dtype=np.uint8)
        full_rgba[:, :3] = res_rgb.astype(np.uint8)
        
        # Fast checksum calculation
        # FNV-1a is inherently sequential, but we can speed up the loop
        # by processing in chunks or using a more efficient loop if possible.
        # However, standard Python loop is the bottleneck. 
        # Let's use the bytearray directly for the checksum to match the original logic.
        
        data = full_rgba.tobytes()
        h = h_const
        for b in data:
            h ^= b
            h = (h * prime) & mask
        last_h = h
        
    return last_h