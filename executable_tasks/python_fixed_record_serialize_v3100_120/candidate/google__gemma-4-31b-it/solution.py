import struct

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    # Using a local variable for the multiplier to speed up lookup
    mult = 1099511628211
    for b in data:
        h = ((h ^ b) * mult) & mask
    return h

def run(ids, ts, levels, lens, iters: int) -> int:
    # Pre-calculate the format string for the entire batch of records
    # I: unsigned int (4 bytes), H: unsigned short (2 bytes)
    # Each record is 4+4+2+2 = 12 bytes
    count = len(ids)
    fmt = f'<{count * 4}I' # This is a simplification; we need to interleave
    
    # To interleave correctly and efficiently, we flatten the lists
    # However, the most performant way in Python for fixed records is often 
    # to use struct.pack with a generated format string for the whole block
    # Format: (I I H H) * count
    record_fmt = '<IIHH'
    full_fmt = record_fmt * count
    
    # Prepare the data tuple for packing
    # We zip the lists to create a flat sequence of values
    data_to_pack = []
    for i in range(count):
        data_to_pack.append(int(ids[i]))
        data_to_pack.append(int(ts[i]))
        data_to_pack.append(int(levels[i]))
        data_to_pack.append(int(lens[i]))
    
    # Convert to tuple once to avoid repeated list access in the loop
    data_tuple = tuple(data_to_pack)
    
    h = 0
    for _ in range(iters):
        # Pack all records into a single bytes object
        out = struct.pack(full_fmt, *data_tuple)
        h = checksum_bytes(out)
        
    return h