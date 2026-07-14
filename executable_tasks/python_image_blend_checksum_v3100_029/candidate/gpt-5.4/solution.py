def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Precompute alpha-dependent terms once per source pixel.
    inv = [0] * pixels
    alpha = [0] * pixels
    for p in range(pixels):
        a = src[p * 4 + 3]
        alpha[p] = a
        inv[p] = 255 - a

    out = bytearray(n)
    mask = (1 << 64) - 1
    h = 0

    for _ in range(iters):
        j = 0
        p = 0
        while j < n:
            a = alpha[p]
            ia = inv[p]

            s0 = src[j]
            d0 = dst[j]
            out[j] = ((s0 * a) + (d0 * ia) + 127) // 255
            j1 = j + 1
            s1 = src[j1]
            d1 = dst[j1]
            out[j1] = ((s1 * a) + (d1 * ia) + 127) // 255
            j2 = j + 2
            s2 = src[j2]
            d2 = dst[j2]
            out[j2] = ((s2 * a) + (d2 * ia) + 127) // 255
            out[j + 3] = 255

            j += 4
            p += 1

        hh = 1469598103934665603
        for b in out:
            hh ^= b
            hh = (hh * 1099511628211) & mask
        h = hh

    return h
