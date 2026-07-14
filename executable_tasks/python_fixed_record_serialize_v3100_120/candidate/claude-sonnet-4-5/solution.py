import struct

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    # Process in chunks for better cache locality
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    n = len(ids)
    record_size = 12  # 4 + 4 + 2 + 2 bytes
    total_size = n * record_size
    
    # Pre-allocate buffer once
    out = bytearray(total_size)
    
    # Pre-compile struct format for a single record
    # '<' = little-endian, 'I' = uint32, 'H' = uint16
    fmt = '<IIHH'
    pack_into = struct.pack_into
    
    h = 0
    for _ in range(iters):
        # Write all records directly into buffer
        offset = 0
        for i in range(n):
            pack_into(fmt, out, offset, ids[i], ts[i], levels[i], lens[i])
            offset += record_size
        
        h = checksum_bytes(out)
    
    return h