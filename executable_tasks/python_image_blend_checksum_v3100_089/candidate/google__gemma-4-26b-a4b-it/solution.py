import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    # Using memoryview for faster access to bytes
    mv = memoryview(data)
    for b in mv:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Convert inputs to numpy arrays for vectorized operations
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4).astype(np.int32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4).astype(np.int32)
    
    # Pre-extract alpha and calculate weights to avoid repeated work
    # a is src[base + 3]
    alphas = src_arr[:, 3].astype(np.int32)
    inv_alphas = 255 - alphas
    
    # Pre-allocate output array
    # We use int32 for intermediate math to prevent overflow before the // 255
    out_arr = np.empty((pixels, 4), dtype=np.uint8)
    
    # The formula: ((s * a) + (d * (255 - a)) + 127) // 255
    # We only apply this to RGB channels (0, 1, 2)
    # Alpha channel is always 255
    
    for _ in range(iters):
        # Vectorized blend for RGB
        # Reshape alphas for broadcasting: (pixels, 1)
        a_col = alphas[:, np.newaxis]
        ia_col = inv_alphas[:, np.newaxis]
        
        # Perform the math on the first 3 channels
        rgb_blend = (src_arr[:, :3] * a_col + dst_arr[:, :3] * ia_col + 127) // 255
        
        # Assign to output
        out_arr[:, :3] = rgb_blend.astype(np.uint8)
        out_arr[:, 3] = 255
        
        # Convert back to bytes for checksum
        # tobytes() is very fast for numpy arrays
        current_out_bytes = out_arr.tobytes()
        h = checksum_bytes(current_out_bytes)
        
    return h