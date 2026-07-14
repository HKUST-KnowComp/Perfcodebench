import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int len = width * height * 4;
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int base = 0; base < len; base += 4) {
        final int a = src[base + 3] & 0xFF;
        final int invA = 255 - a;

        int v = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        h ^= (long) v;
        h *= FNV_PRIME;

        v = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        h ^= (long) v;
        h *= FNV_PRIME;

        v = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        h ^= (long) v;
        h *= FNV_PRIME;

        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h;
    }

    return hash;
  }
}
