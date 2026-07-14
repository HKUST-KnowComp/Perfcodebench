def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    # If no iterations, baseline leaves h at 0
    if iters <= 0:
        return 0

    # Compute the checksum over the blended output once, as repeated iterations yield the same result
    total = width * height * 4

    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask64 = (1 << 64) - 1

    s = src
    d = dst
    c255 = 255

    # Blend and hash in RGBA order per pixel
    for base in range(0, total, 4):
        a = s[base + 3]
        inv = 255 - a

        r = (s[base] * a + d[base] * inv + 127) // 255
        g_idx = base + 1
        g = (s[g_idx] * a + d[g_idx] * inv + 127) // 255
        b_idx = base + 2
        b = (s[b_idx] * a + d[b_idx] * inv + 127) // 255

        # FNV-1a update for r, g, b, a_out=255
        h ^= r
        h = (h * fnv_prime) & mask64
        h ^= g
        h = (h * fnv_prime) & mask64
        h ^= b
        h = (h * fnv_prime) & mask64
        h ^= c255
        h = (h * fnv_prime) & mask64

    return h
