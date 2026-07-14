import struct

# Optional helper retained for reference; not used in the hot path
def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    for b in data:
        h ^= b
        h = (h * prime) & mask
    return h


def run(ids, ts, levels, lens, iters: int) -> int:
    # Cache frequently used locals and prepare a single reusable buffer
    ids0 = ids
    ts0 = ts
    levels0 = levels
    lens0 = lens

    n = len(ids0)
    s = struct.Struct('<IIHH')
    rec_size = s.size  # 12 bytes per record
    total_size = n * rec_size
    out = bytearray(total_size)
    pack_into = s.pack_into

    # Constants for FNV-1a 64-bit
    prime = 1099511628211
    mask = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        # Serialize all records into the preallocated buffer
        o = 0
        for i in range(n):
            pack_into(out, o, ids0[i], ts0[i], levels0[i], lens0[i])
            o += rec_size

        # Compute checksum over the resulting buffer
        hv = 1469598103934665603
        for b in out:
            hv ^= b
            hv = (hv * prime) & mask
        h = hv

    return h
