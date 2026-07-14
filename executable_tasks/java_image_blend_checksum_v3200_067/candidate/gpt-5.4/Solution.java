import java.util.*;

public class Solution {
  private static long checksumBlended(byte[] src, byte[] dst, int pixels) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;
    int base = 0;
    for (int i = 0; i < pixels; i++, base += 4) {
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;

      int s0 = src[base] & 0xFF;
      int d0 = dst[base] & 0xFF;
      int v0 = (s0 * a + d0 * invA + 127) / 255;
      hash ^= (long) v0;
      hash *= prime;

      int s1 = src[base + 1] & 0xFF;
      int d1 = dst[base + 1] & 0xFF;
      int v1 = (s1 * a + d1 * invA + 127) / 255;
      hash ^= (long) v1;
      hash *= prime;

      int s2 = src[base + 2] & 0xFF;
      int d2 = dst[base + 2] & 0xFF;
      int v2 = (s2 * a + d2 * invA + 127) / 255;
      hash ^= (long) v2;
      hash *= prime;

      hash ^= 255L;
      hash *= prime;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    long hash = 0L;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      hash = checksumBlended(src, dst, pixels);
    }
    return hash;
  }
}
