import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Preallocate output
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, 3] = 255
    
    # Precompute blend for RGB channels using uint16 to avoid overflow
    src_rgb = src_arr[:, :3].astype(np.uint16)
    dst_rgb = dst_arr[:, :3].astype(np.uint16)
    
    # Blend: ((s * a) + (d * (255 - a)) + 127) // 255
    alpha_3d = alpha[:, np.newaxis]
    inv_alpha_3d = inv_alpha[:, np.newaxis]
    
    blended = (src_rgb * alpha_3d + dst_rgb * inv_alpha_3d + 127) // 255
    out[:, :3] = blended.astype(np.uint8)
    
    # Flatten for checksum
    out_flat = out.ravel()
    
    # FNV-1a checksum constants
    h = np.uint64(1469598103934665603)
    prime = np.uint64(1099511628211)
    
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for b in out_flat:
            h ^= np.uint64(b)
            h = (h * prime) & np.uint64((1 << 64) - 1)
    
    return int(h)
