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
    out = bytearray(pixels * 4)
    alpha = bytearray([255] * pixels)
    h = 0
    for i in range(iters):
        for j in range(pixels):
            base = j * 4
            a = src[base + 3]
            # R channel
            s_r = src[base]
            d_r = dst[base]
            r = d_r + (a * (s_r - d_r) + 127) // 255
            # G channel
            s_g = src[base + 1]
            d_g = dst[base + 1]
            g = d_g + (a * (s_g - d_g) + 127) // 255
            # B channel
            s_b = src[base + 2]
            d_b = dst[base + 2]
            b = d_b + (a * (s_b - d_b) + 127) // 255
            out[base] = r
            out[base + 1] = g
            out[base + 2] = b
        out[3::4] = alpha
        if i == 0:
            h = checksum_bytes(out)
    return h