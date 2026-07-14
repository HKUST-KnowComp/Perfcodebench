import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # Convert to numpy arrays for vectorized math
    # Use uint32 to prevent overflow during intermediate multiplication
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(pixels, 4).astype(np.uint32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(pixels, 4).astype(np.uint32)
    
    # Pre-calculate alpha and inverse alpha
    a = src_arr[:, 3:4]
    inv_a = 255 - a
    
    # Prepare output array
    # We only need to compute RGB; A is always 255
    out_arr = np.empty((pixels, 4), dtype=np.uint32)
    
    # Vectorized blend: ((s * a) + (d * (255 - a)) + 127) // 255
    # We only apply this to the first 3 channels (RGB)
    out_arr[:, :3] = ((src_arr[:, :3] * a) + (dst_arr[:, :3] * inv_a) + 127) // 255
    out_arr[:, 3] = 255
    
    # Convert back to uint8 for the checksum
    out_bytes = out_arr.astype(np.uint8).tobytes()
    
    h = 0
    for _ in range(iters):
        h = checksum_bytes(out_bytes)
        
    return h