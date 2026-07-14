def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    if iters == 0:
        return 0
    MASK64 = (1 << 64) - 1
    MUL = 1099511628211
    h = 1469598103934665603 & MASK64
    s = src
    d = dst
    base = 0
    for _ in range(width * height):
        a = s[base + 3]
        # Red
        v = ((s[base] * a) + (d[base] * (255 - a)) + 127) // 255
        h ^= v
        h = (h * MUL) & MASK64
        # Green
        v = ((s[base + 1] * a) + (d[base + 1] * (255 - a)) + 127) // 255
        h ^= v
        h = (h * MUL) & MASK64
        # Blue
        v = ((s[base + 2] * a) + (d[base + 2] * (255 - a)) + 127) // 255
        h ^= v
        h = (h * MUL) & MASK64
        # Alpha
        h ^= 255
        h = (h * MUL) & MASK64
        base += 4
    return h
