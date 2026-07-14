import numpy as np

def checksum_bytes(data: bytes) -> int:
    h = 146959810394665603
    # Note: The original baseline had a typo in the constant 1469598103934665603
    # but we must match the logic provided in the baseline exactly.
    # Re-implementing the exact baseline logic for correctness.
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized operations
    src_arr = np.frombuffer(src, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    dst_arr = np.frombuffer(dst, dtype=np.uint8).reshape(-1, 4).astype(np.int32)
    
    # Pre-calculate constants
    # We only blend RGB, Alpha is always 255
    # Formula: ((s * a) + (d * (255 - a)) + 127) // 255
    
    # Prepare output array
    out_arr = np.empty_like(src_arr)
    out_arr[:, 3] = 255
    
    last_checksum = 0
    
    for _ in range(iters):
        a = src_arr[:, 3:4]  # Keep dimensions for broadcasting
        inv_a = 255 - a
        
        # Vectorized blend for RGB channels
        # We use int32 to prevent overflow during multiplication
        out_arr[:, :3] = ((src_arr[:, :3] * a) + (dst_arr[:, :3] * inv_a) + 127) // 255
        
        # Convert back to bytes for checksum
        # The baseline uses out.extend which builds a bytearray
        # We need to ensure the output is exactly the same byte sequence
        out_bytes = out_arr.astype(np.uint8).tobytes()
        
        # Calculate checksum using the exact baseline logic
        h = 1469598103934665603
        for b in out_bytes:
            h ^= b
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        last_checksum = h
        
    return last_checksum