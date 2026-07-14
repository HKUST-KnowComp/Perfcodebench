public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int n = pixels << 2; // total bytes per image
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int i = 0; i < n; i += 4) {
        final int a = src[i + 3] & 0xFF;
        final int invA = 255 - a;

        int s = src[i] & 0xFF;
        int d = dst[i] & 0xFF;
        int t = s * a + d * invA + 127;
        int r = t / 255;
        h ^= (long) r;
        h *= FNV_PRIME;

        s = src[i + 1] & 0xFF;
        d = dst[i + 1] & 0xFF;
        t = s * a + d * invA + 127;
        int g = t / 255;
        h ^= (long) g;
        h *= FNV_PRIME;

        s = src[i + 2] & 0xFF;
        d = dst[i + 2] & 0xFF;
        t = s * a + d * invA + 127;
        int b = t / 255;
        h ^= (long) b;
        h *= FNV_PRIME;

        // Output alpha is always 255
        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h; // checksum of the last iteration's blended output
    }
    return hash;
  }
}
