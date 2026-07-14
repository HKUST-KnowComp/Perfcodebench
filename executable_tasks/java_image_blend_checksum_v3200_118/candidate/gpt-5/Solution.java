public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    int pixels = width * height;
    int n = pixels * 4;
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int base = 0; base < n; base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        int v = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        v = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        v = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        h ^= (long) (v & 0xFF);
        h *= FNV_PRIME;

        // Alpha is forced to 255 in the output
        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h; // checksum of the last iteration's blended image
    }

    return hash;
  }
}
