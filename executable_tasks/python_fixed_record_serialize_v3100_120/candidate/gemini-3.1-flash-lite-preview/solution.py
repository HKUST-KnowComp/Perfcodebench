import struct

def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h

def run(ids, ts, levels, lens, iters: int) -> int:
    # Pre-calculate record size and format string
    # 4 bytes (I) + 4 bytes (I) + 2 bytes (H) + 2 bytes (H) = 12 bytes
    record_format = '<IIHH'
    record_size = 12
    num_records = len(ids)
    out = bytearray(num_records * record_size)
    
    h = 0
    # Zip the iterables once to avoid repeated indexing
    data_zipped = list(zip(ids, ts, levels, lens))
    
    for _ in range(iters):
        for i, (id_val, ts_val, lvl_val, len_val) in enumerate(data_zipped):
            struct.pack_into(record_format, out, i * record_size, id_val, ts_val, lvl_val, len_val)
        h = checksum_bytes(out)
    return h