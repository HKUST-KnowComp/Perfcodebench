def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels4 = width * height * 4
    mask64 = (1 << 64) - 1
    fnv_prime = 1099511628211
    fnv_offset = 1469598103934665603

    s = src
    d = dst
    h = 0

    for _ in range(iters):
        hh = fnv_offset
        i = 0
        while i < pixels4:
            a = s[i + 3]
            inv = 255 - a

            b = ((s[i] * a) + (d[i] * inv) + 127) // 255
            hh ^= b
            hh = (hh * fnv_prime) & mask64

            b = ((s[i + 1] * a) + (d[i + 1] * inv) + 127) // 255
            hh ^= b
            hh = (hh * fnv_prime) & mask64

            b = ((s[i + 2] * a) + (d[i + 2] * inv) + 127) // 255
            hh ^= b
            hh = (hh * fnv_prime) & mask64

            hh ^= 255
            hh = (hh * fnv_prime) & mask64

            i += 4
        h = hh
    return h
