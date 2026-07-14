import numpy as np

def blend_byte(s: int, d: int, a: int) -> int:
    return ((s * a) + (d * (255 - a)) + 127) // 255


def checksum_bytes(data: bytes) -> int:
    h = 1469598103934665603
    for b in data:
        h ^= b
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h


def run(src: bytearray, dst: bytearray, width: int, height: int, iters: int) -> int:
    pixels = width * height
    # pre-allocate the output buffer once to avoid repeated malloc/free
    out = bytearray(pixels * 4)

    # create numpy views over the input buffers without copying the data
    src_np = np.frombuffer(src, dtype=np.uint8).reshape((pixels, 4))
    dst_np = np.frombuffer(dst, dtype=np.uint8).reshape((pixels, 4))

    # use a small int32 buffer for the math so no saturation issues occur
    src32 = src_np.astype(np.int32)
    dst32 = dst_np.astype(np.int32)

    # vectorised blend over the entire image
    def blend_pass():
        # pull out the alpha column once
        a = src32[:, 3:4]  # (pixels,1)
        # blend the RGB channels
        blended = (src32[:, :3] * a + dst32[:, :3] * (255 - a) + 127) // 255
        # convert back to uint8
        blended = blended.astype(np.uint8)
        # copy into the output buffer; set alpha=255
        out_np = np.frombuffer(out, dtype=np.uint8).reshape((pixels, 4))
        out_np[:, :3] = blended
        out_np[:, 3] = 255

    for _ in range(iters):
        blend_pass()

    return checksum_bytes(out)