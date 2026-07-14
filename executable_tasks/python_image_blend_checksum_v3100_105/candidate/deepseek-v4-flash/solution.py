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
    h = 0
    # local alias for speed
    _src = src
    _dst = dst
    _pixels = pixels
    _iters = iters
    FNVI = 1469598103934665603
    FNVPRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    for _ in range(_iters):
        h = FNVI
        i = 0
        while i < _pixels:
            base = i << 2
            a = _src[base + 3]
            # red
            r = ((_src[base] - _dst[base]) * a + _dst[base] * 255 + 127) // 255
            h ^= r
            h = (h * FNVPRIME) & MASK
            # green
            g = ((_src[base + 1] - _dst[base + 1]) * a + _dst[base + 1] * 255 + 127) // 255
            h ^= g
            h = (h * FNVPRIME) & MASK
            # blue
            b = ((_src[base + 2] - _dst[base + 2]) * a + _dst[base + 2] * 255 + 127) // 255
            h ^= b
            h = (h * FNVPRIME) & MASK
            # alpha
            h ^= 255
            h = (h * FNVPRIME) & MASK
            i += 1
    return h