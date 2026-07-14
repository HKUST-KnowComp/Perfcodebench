def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # FNV-1a constants
    OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    s = src
    d = dst

    h = 0
    pr = FNV_PRIME
    m = MASK

    for _ in range(iters):
        hv = OFFSET_BASIS
        # Iterate over pixels (4 bytes per pixel)
        for base in range(0, n, 4):
            a = s[base + 3]
            inv_a = 255 - a

            val = (s[base] * a + d[base] * inv_a + 127) // 255
            hv = ((hv ^ val) * pr) & m

            val = (s[base + 1] * a + d[base + 1] * inv_a + 127) // 255
            hv = ((hv ^ val) * pr) & m

            val = (s[base + 2] * a + d[base + 2] * inv_a + 127) // 255
            hv = ((hv ^ val) * pr) & m

            # Alpha is forced to 255 in output
            hv = ((hv ^ 255) * pr) & m
        h = hv
    return h
