import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    long hash = 0;
    int len = width * height * 4;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int base = 0; base < len; base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        int r = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        h ^= r;
        h *= FNV_PRIME;

        int g = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        h ^= g;
        h *= FNV_PRIME;

        int b = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        h ^= b;
        h *= FNV_PRIME;

        h ^= 255;
        h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}