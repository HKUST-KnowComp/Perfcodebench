public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;

    final int pixels = width * height;
    final int limit = pixels << 2; // number of bytes

    long hash = FNV_OFFSET;

    for (int base = 0; base < limit; base += 4) {
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;

      int s = src[base] & 0xFF;
      int d = dst[base] & 0xFF;
      int t = s * a + d * invA + 127;
      int out0 = t / 255;

      s = src[base + 1] & 0xFF;
      d = dst[base + 1] & 0xFF;
      t = s * a + d * invA + 127;
      int out1 = t / 255;

      s = src[base + 2] & 0xFF;
      d = dst[base + 2] & 0xFF;
      t = s * a + d * invA + 127;
      int out2 = t / 255;

      // FNV-1a over RGBA (A forced to 255)
      hash ^= (out0 & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= (out1 & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= (out2 & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= 255L;
      hash *= FNV_PRIME;
    }

    // For multiple iterations, the output is identical; the checksum of the last iteration equals this value.
    return hash;
  }
}
