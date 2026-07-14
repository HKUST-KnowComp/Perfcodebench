public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int totalBytes = pixels << 2;
    long hash = 0L;
    if (iters <= 0) return 0L;

    final long prime = FNV64_PRIME;
    final byte[] s = src;
    final byte[] d = dst;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV64_OFFSET;
      int i = 0;
      while (i < totalBytes) {
        int s0 = s[i] & 0xFF;
        int d0 = d[i] & 0xFF;
        int s1 = s[i + 1] & 0xFF;
        int d1 = d[i + 1] & 0xFF;
        int s2 = s[i + 2] & 0xFF;
        int d2 = d[i + 2] & 0xFF;
        int a = s[i + 3] & 0xFF;
        int inv = 255 - a;

        int v0 = s0 * a + d0 * inv;
        int out0 = (v0 + 127) / 255;
        h ^= (long) (out0 & 0xFF);
        h *= prime;

        int v1 = s1 * a + d1 * inv;
        int out1 = (v1 + 127) / 255;
        h ^= (long) (out1 & 0xFF);
        h *= prime;

        int v2 = s2 * a + d2 * inv;
        int out2 = (v2 + 127) / 255;
        h ^= (long) (out2 & 0xFF);
        h *= prime;

        h ^= 255L; // alpha channel is 255
        h *= prime;

        i += 4;
      }
      hash = h; // checksum of the last iteration's output
    }
    return hash;
  }
}
