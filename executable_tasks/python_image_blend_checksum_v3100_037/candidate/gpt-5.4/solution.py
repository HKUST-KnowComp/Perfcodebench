def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Reuse output buffer across iterations.
    out = bytearray(n)

    # Precompute alpha-dependent terms once per pixel.
    alphas = [0] * pixels
    inv_alphas = [0] * pixels
    bases = [0] * pixels
    j = 0
    for i in range(pixels):
        bases[i] = j
        a = src[j + 3]
        alphas[i] = a
        inv_alphas[i] = 255 - a
        j += 4

    mask = (1 << 64) - 1
    h = 0

    for _ in range(iters):
        # Blend directly into preallocated output.
        for i in range(pixels):
            base = bases[i]
            a = alphas[i]
            ia = inv_alphas[i]

            out[base] = ((src[base] * a) + (dst[base] * ia) + 127) // 255
            b1 = base + 1
            out[b1] = ((src[b1] * a) + (dst[b1] * ia) + 127) // 255
            b2 = base + 2
            out[b2] = ((src[b2] * a) + (dst[b2] * ia) + 127) // 255
            out[base + 3] = 255

        # Inline checksum to avoid function-call overhead.
        hh = 1469598103934665603
        for b in out:
            hh ^= b
            hh = (hh * 1099511628211) & mask
        h = hh

    return h
