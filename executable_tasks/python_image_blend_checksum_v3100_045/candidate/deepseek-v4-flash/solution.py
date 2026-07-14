def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    pixel_bytes = pixels * 4
    s = src
    d = dst
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        hash_val = fnv_offset
        for base in range(0, pixel_bytes, 4):
            a = s[base + 3]
            inv_a = 255 - a
            # R
            b = (s[base] * a + d[base] * inv_a + 127) // 255
            hash_val ^= b
            hash_val = (hash_val * fnv_prime) & mask
            # G
            b = (s[base + 1] * a + d[base + 1] * inv_a + 127) // 255
            hash_val ^= b
            hash_val = (hash_val * fnv_prime) & mask
            # B
            b = (s[base + 2] * a + d[base + 2] * inv_a + 127) // 255
            hash_val ^= b
            hash_val = (hash_val * fnv_prime) & mask
            # A (always 255)
            hash_val ^= 255
            hash_val = (hash_val * fnv_prime) & mask
        h = hash_val
    return h