def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Preserve baseline behavior: if no iterations, return 0
    if iters <= 0:
        return 0

    pixels = width * height

    # FNV-1a 64-bit constants
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    s = src  # local binding for speed
    d = dst

    base = 0
    for _ in range(pixels):
        a = s[base + 3]
        inv = 255 - a

        # R channel
        v = (s[base] * a + d[base] * inv + 127) // 255
        h ^= v
        h = (h * prime) & mask

        # G channel
        v = (s[base + 1] * a + d[base + 1] * inv + 127) // 255
        h ^= v
        h = (h * prime) & mask

        # B channel
        v = (s[base + 2] * a + d[base + 2] * inv + 127) // 255
        h ^= v
        h = (h * prime) & mask

        # A channel is forced to 255 in the output
        h ^= 255
        h = (h * prime) & mask

        base += 4

    return h
