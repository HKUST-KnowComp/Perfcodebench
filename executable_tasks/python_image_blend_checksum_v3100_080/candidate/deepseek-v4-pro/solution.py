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
    if pixels == 0:
        return 1469598103934665603
    s = src
    d = dst
    initial_h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    for _ in range(iters):
        h = initial_h
        for i in range(pixels):
            base = i << 2
            a = s[base + 3]
            inv_a = 255 - a
            # blend R
            b_val = ((s[base] * a) + (d[base] * inv_a) + 127) // 255
            h ^= b_val
            h = (h * mult) & mask
            # blend G
            b_val = ((s[base + 1] * a) + (d[base + 1] * inv_a) + 127) // 255
            h ^= b_val
            h = (h * mult) & mask
            # blend B
            b_val = ((s[base + 2] * a) + (d[base + 2] * inv_a) + 127) // 255
            h ^= b_val
            h = (h * mult) & mask
            # alpha = 255
            h ^= 255
            h = (h * mult) & mask
    return h
