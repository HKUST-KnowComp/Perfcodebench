def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    out = bytearray(pixels * 4)          # pre-allocate once
    sv = memoryview(src).cast('B')
    dv = memoryview(dst).cast('B')
    ov = memoryview(out).cast('B')

    for _ in range(iters):
        for i in range(pixels):
            base = i * 4
            a = sv[base + 3]
            ov[base]     = blend_byte(sv[base],     dv[base],     a)
            ov[base + 1] = blend_byte(sv[base + 1], dv[base + 1], a)
            ov[base + 2] = blend_byte(sv[base + 2], dv[base + 2], a)
            ov[base + 3] = 255
    return checksum_bytes(out)