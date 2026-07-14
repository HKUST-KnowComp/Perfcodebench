import numpy as np

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    
    # Convert to numpy arrays and reshape to (pixels, 4)
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4)
    
    # Extract alpha channel from source
    alpha = src_arr[:, 3].astype(np.uint16)
    inv_alpha = 255 - alpha
    
    # Precompute blended RGB channels using vectorized operations
    # Use uint16 to avoid overflow: max value is 255*255 + 255*255 + 127 = 130177
    src_rgb = src_arr[:, :3].astype(np.uint16)
    dst_rgb = dst_arr[:, :3].astype(np.uint16)
    
    # Blend: ((s * a) + (d * (255 - a)) + 127) // 255
    blended_rgb = ((src_rgb * alpha[:, np.newaxis]) + 
                   (dst_rgb * inv_alpha[:, np.newaxis]) + 127) // 255
    blended_rgb = blended_rgb.astype(np.uint8)
    
    # Create output array with alpha = 255
    out = np.empty((pixels, 4), dtype=np.uint8)
    out[:, :3] = blended_rgb
    out[:, 3] = 255
    
    # Flatten to bytes for checksum
    out_flat = out.ravel()
    
    # FNV-1a checksum constants
    h = np.uint64(1469598103934665603)
    prime = np.uint64(1099511628211)
    
    # Compute checksum - need to iterate but can use numpy scalar ops
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for b in out_flat:
            h ^= np.uint64(b)
            h = (h * prime)
    
    return int(h)
