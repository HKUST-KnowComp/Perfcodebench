MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Fast local bindings
    src_b = src
    dst_b = dst
    mask64 = MASK64
    prime = FNV_PRIME
    offset = FNV_OFFSET

    h = 0
    for _ in range(iters):
        hh = offset
        i = 0
        while i < n:
            a = src_b[i + 3]
            inv_a = 255 - a

            b = ((src_b[i] * a) + (dst_b[i] * inv_a) + 127) // 255
            hh ^= b
            hh = (hh * prime) & mask64

            j = i + 1
            b = ((src_b[j] * a) + (dst_b[j] * inv_a) + 127) // 255
            hh ^= b
            hh = (hh * prime) & mask64

            j = i + 2
            b = ((src_b[j] * a) + (dst_b[j] * inv_a) + 127) // 255
            hh ^= b
            hh = (hh * prime) & mask64

            hh ^= 255
            hh = (hh * prime) & mask64

            i += 4
        h = hh
    return h
