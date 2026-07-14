MASK64 = (1 << 64) - 1


def checksum_bytes(data) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & MASK64
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    n = width * height * 4
    out = bytearray(n)
    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    h = 0
    for _ in range(iters):
        s = src_mv
        d = dst_mv
        o = out_mv

        for base in range(0, n, 4):
            a = s[base + 3]
            inv = 255 - a
            o[base] = (s[base] * a + d[base] * inv + 127) // 255
            b1 = base + 1
            o[b1] = (s[b1] * a + d[b1] * inv + 127) // 255
            b2 = base + 2
            o[b2] = (s[b2] * a + d[b2] * inv + 127) // 255
            o[base + 3] = 255

        h = 1469598103934665603
        for b in o:
            h ^= b
            h = (h * 1099511628211) & MASK64
    return h
