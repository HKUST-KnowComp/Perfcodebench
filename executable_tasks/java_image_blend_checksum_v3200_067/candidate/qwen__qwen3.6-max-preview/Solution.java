import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = src.length;
    long hash = 0;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      hash = FNV_OFFSET;
      for (int base = 0; base < len; base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        int r = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        hash ^= r;
        hash *= FNV_PRIME;

        int g = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        hash ^= g;
        hash *= FNV_PRIME;

        int b = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        hash ^= b;
        hash *= FNV_PRIME;

        hash ^= 255;
        hash *= FNV_PRIME;
      }
    }
    return hash;
  }
}