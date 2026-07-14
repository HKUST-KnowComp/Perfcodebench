def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Compute checksum directly while blending to avoid constructing output buffer.
    pixels = width * height
    total_bytes = pixels * 4

    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    h = 0
    src_b = src  # local alias
    dst_b = dst

    for _ in range(iters):
        h = offset
        i = 0
        while i < total_bytes:
            a = src_b[i + 3]
            inv_a = 255 - a

            # R channel
            s = src_b[i]
            d = dst_b[i]
            r = (s * a + d * inv_a + 127) // 255

            # G channel
            s = src_b[i + 1]
            d = dst_b[i + 1]
            g = (s * a + d * inv_a + 127) // 255

            # B channel
            s = src_b[i + 2]
            d = dst_b[i + 2]
            b = (s * a + d * inv_a + 127) // 255

            # FNV-1a hash update for r, g, b, and alpha (255)
            h ^= r
            h = (h * prime) & mask
            h ^= g
            h = (h * prime) & mask
            h ^= b
            h = (h * prime) & mask
            h ^= 255
            h = (h * prime) & mask

            i += 4

    return h
