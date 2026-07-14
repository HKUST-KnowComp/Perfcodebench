public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    // Match baseline behavior: if iters == 0, return 0
    if (iters <= 0) return 0L;

    final int pixels = width * height;
    final int totalBytes = pixels * 4;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    long hash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int base = 0; base < totalBytes; base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        int s = src[base] & 0xFF;
        int d = dst[base] & 0xFF;
        int v = (s * a + d * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        s = src[base + 1] & 0xFF;
        d = dst[base + 1] & 0xFF;
        v = (s * a + d * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        s = src[base + 2] & 0xFF;
        d = dst[base + 2] & 0xFF;
        v = (s * a + d * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        // Alpha channel is set to 255 in the output
        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h; // same as baseline: keep last iteration's checksum
    }
    return hash;
  }
}