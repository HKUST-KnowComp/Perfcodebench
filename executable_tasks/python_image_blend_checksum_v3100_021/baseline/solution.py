def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    out = bytearray()
    pixels = width * height
    h = 0
    for _ in range(iters):
        out.clear()
        for i in range(pixels):
            base = i * 4
            a = src[base + 3]
            out.extend((
                blend_byte(src[base], dst[base], a),
                blend_byte(src[base + 1], dst[base + 1], a),
                blend_byte(src[base + 2], dst[base + 2], a),
                255,
            ))
        h = checksum_bytes(out)
    return h
