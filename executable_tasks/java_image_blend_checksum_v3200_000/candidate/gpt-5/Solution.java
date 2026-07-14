public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int end = pixels << 2; // number of bytes (RGBA)
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV64_OFFSET;
      for (int base = 0; base < end; base += 4) {
        final int a = src[base + 3] & 0xFF;
        final int invA = 255 - a;

        int s = src[base] & 0xFF;
        int d = dst[base] & 0xFF;
        int blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV64_PRIME;

        s = src[base + 1] & 0xFF;
        d = dst[base + 1] & 0xFF;
        blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV64_PRIME;

        s = src[base + 2] & 0xFF;
        d = dst[base + 2] & 0xFF;
        blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV64_PRIME;

        // Alpha channel: always 255 in output
        h ^= 255L;
        h *= FNV64_PRIME;
      }
      hash = h; // only the last iteration's checksum is returned
    }
    return hash;
  }
}
