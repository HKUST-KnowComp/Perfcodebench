def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # Constants for FNV-1a 64-bit
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    pixels = width * height
    total_bytes = pixels * 4

    # Local bindings for speed
    s = src
    d = dst
    fnv_prime = FNV_PRIME
    mask = MASK64
    offset = FNV_OFFSET_BASIS
    div = 255

    h_out = 0

    for _ in range(iters):
        h = offset
        j = 0
        while j < total_bytes:
            a = s[j + 3]
            inva = 255 - a

            # R
            v = ((s[j] * a) + (d[j] * inva) + 127) // div
            h ^= v
            h = (h * fnv_prime) & mask

            # G
            v = ((s[j + 1] * a) + (d[j + 1] * inva) + 127) // div
            h ^= v
            h = (h * fnv_prime) & mask

            # B
            v = ((s[j + 2] * a) + (d[j + 2] * inva) + 127) // div
            h ^= v
            h = (h * fnv_prime) & mask

            # A is always 255 in output
            h ^= 255
            h = (h * fnv_prime) & mask

            j += 4

        h_out = h

    return h_out
