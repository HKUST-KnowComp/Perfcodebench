import struct
import numpy as np

def run(ids, ts, levels, lens, iters: int) -> int:
    n = len(ids)
    if n == 0:
        # Empty case: checksum of empty bytes
        return 1469598103934665603

    # Convert inputs to numpy arrays for vectorized packing
    ids_arr = np.asarray(ids, dtype=np.uint32)
    ts_arr = np.asarray(ts, dtype=np.uint32)
    levels_arr = np.asarray(levels, dtype=np.uint16)
    lens_arr = np.asarray(lens, dtype=np.uint16)

    # Build the binary buffer using struct for all records at once
    # Each record: 4 bytes (id) + 4 bytes (ts) + 2 bytes (level) + 2 bytes (len) = 12 bytes
    record_size = 12
    total_size = n * record_size
    
    # Precompile struct format for all records
    fmt = '<' + 'IIHH' * n
    
    # Interleave the arrays for struct.pack
    # Create a flat sequence: id0, ts0, level0, len0, id1, ts1, level1, len1, ...
    values = []
    for i in range(n):
        values.extend([int(ids_arr[i]), int(ts_arr[i]), int(levels_arr[i]), int(lens_arr[i])])
    
    # Actually, let's use a more efficient approach with numpy structured array
    # Create a structured array and get its bytes directly
    dtype = np.dtype([('id', '<u4'), ('ts', '<u4'), ('level', '<u2'), ('len', '<u2')])
    records = np.empty(n, dtype=dtype)
    records['id'] = ids_arr
    records['ts'] = ts_arr
    records['level'] = levels_arr
    records['len'] = lens_arr
    
    out = records.tobytes()
    
    # Vectorized FNV-1a checksum
    # FNV-1a: for each byte b: h ^= b; h *= 1099511628211; h &= mask
    # We need to process byte by byte but can use numpy for speed
    
    h = np.uint64(1469598103934665603)
    prime = np.uint64(1099511628211)
    
    # Process as numpy array of bytes
    data = np.frombuffer(out, dtype=np.uint8)
    
    # Unfortunately FNV-1a is inherently sequential, but numpy loop is faster than pure Python
    for b in data:
        h ^= np.uint64(b)
        h *= prime
    
    # For iterations > 1, we just repeat (buffer is same each time)
    for _ in range(iters - 1):
        h = np.uint64(1469598103934665603)
        for b in data:
            h ^= np.uint64(b)
            h *= prime
    
    return int(h)
