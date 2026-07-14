MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels4 = width * height * 4
    h = 0

    src_mv = memoryview(src)
    dst_mv = memoryview(dst)

    fnv_offset = FNV_OFFSET
    fnv_prime = FNV_PRIME
    mask64 = MASK64

    for _ in range(iters):
        hs = fnv_offset
        for base in range(0, pixels4, 4):
            a = src_mv[base + 3]
            inv = 255 - a

            s = src_mv[base]
            d = dst_mv[base]
            b = ((s * a) + (d * inv) + 127) // 255
            hs ^= b
            hs = (hs * fnv_prime) & mask64

            s = src_mv[base + 1]
            d = dst_mv[base + 1]
            b = ((s * a) + (d * inv) + 127) // 255
            hs ^= b
            hs = (hs * fnv_prime) & mask64

            s = src_mv[base + 2]
            d = dst_mv[base + 2]
            b = ((s * a) + (d * inv) + 127) // 255
            hs ^= b
            hs = (hs * fnv_prime) & mask64

            hs ^= 255
            hs = (hs * fnv_prime) & mask64

        h = hs
    return h
