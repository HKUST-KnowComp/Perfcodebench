import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized processing
    # We use uint8 for the raw data
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4)
    
    # Pre-extract channels to avoid repeated indexing
    # src_rgb: (N, 3), src_a: (N, 1), dst_rgb: (N, 3)
    src_rgb = src_arr[:, :3].astype(np.uint32)
    src_a = src_arr[:, 3:4].astype(np.uint32)
    dst_rgb = dst_arr[:, :3].astype(np.uint32)
    
    # Pre-calculate (255 - a)
    inv_a = 255 - src_a
    
    # The blending formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We perform this in uint32 to prevent overflow during intermediate steps
    blended_rgb = (src_rgb * src_a + dst_rgb * inv_a + 127) // 255
    
    # Construct the output array: [R, G, B, 255]
    # We create an empty array of shape (N, 4) and fill it
    n_pixels = width * height
    out_arr = np.empty((n_pixels, 4), dtype=np.uint8)
    out_arr[:, :3] = blended_rgb.astype(np.uint8)
    out_arr[:, 3] = 255
    
    # Flatten for checksum calculation
    out_flat = out_arr.ravel()
    
    # FNV-1a 64-bit implementation
    # h = 1469598103934665603
    # h = (h ^ b) * 1099511628211 & mask
    h_init = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    
    # Since Python handles arbitrary precision integers, we can't easily vectorize 
    # the FNV hash with NumPy without a custom C extension or Numba. 
    # However, we can optimize the loop by using a more efficient way to iterate.
    # For the purpose of this task, we will use the standard loop but ensure 
    # the data is accessed as efficiently as possible.
    
    h = h_init
    # Using a local variable for the loop and the mask is faster in Python
    for b in out_flat:
        h = ((h ^ b) * prime) & mask
        
    return h
