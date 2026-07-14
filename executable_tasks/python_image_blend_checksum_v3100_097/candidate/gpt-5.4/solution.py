MASK64 = (1 << 64) - 1
FNV_OFFSET = 1469598103934665603
FNV_PRIME = 1099511628211


def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = FNV_OFFSET
    for b in data:
        h ^= b
        h = (h * FNV_PRIME) & MASK64
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    n = pixels * 4

    # Reuse output buffer across iterations.
    out = bytearray(n)

    # Precompute exact rounded division-by-255 blend table.
    # table[a][x] == (x + 127) // 255 for x in [0, 255*255]
    # Flattened for faster indexing: table[a * 65536 + x]
    table = [0] * (256 * 65536)
    for a in range(256):
        base = a << 16
        ia = 255 - a
        row = table
        for s in range(256):
            sa = s * a
            off = base + (s << 8)
            for d in range(256):
                row[off + d] = (sa + d * ia + 127) // 255

    src_mv = memoryview(src)
    dst_mv = memoryview(dst)
    out_mv = memoryview(out)

    h = 0
    for _ in range(iters):
        j = 0
        for i in range(0, n, 4):
            a = src_mv[i + 3]
            tbase = a << 16
            out_mv[j] = table[tbase + (src_mv[i] << 8) + dst_mv[i]]
            out_mv[j + 1] = table[tbase + (src_mv[i + 1] << 8) + dst_mv[i + 1]]
            out_mv[j + 2] = table[tbase + (src_mv[i + 2] << 8) + dst_mv[i + 2]]
            out_mv[j + 3] = 255
            j += 4

        hh = FNV_OFFSET
        for b in out_mv:
            hh ^= b
            hh = (hh * FNV_PRIME) & MASK64
        h = hh

    return h
