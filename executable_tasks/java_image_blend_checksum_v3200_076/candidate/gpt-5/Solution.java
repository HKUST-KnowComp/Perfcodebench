import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = 0L;

    // Local references to help JIT and reduce repeated field loads
    final byte[] sArr = src;
    final byte[] dArr = dst;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      int base = 0;
      for (int i = 0; i < pixels; i++) {
        int a = sArr[base + 3] & 0xFF;
        int oma = 255 - a;

        int s0 = sArr[base] & 0xFF;
        int d0 = dArr[base] & 0xFF;
        int b0 = (s0 * a + d0 * oma + 127) / 255;
        h ^= (b0 & 0xFFL);
        h *= FNV_PRIME;

        int s1 = sArr[base + 1] & 0xFF;
        int d1 = dArr[base + 1] & 0xFF;
        int b1 = (s1 * a + d1 * oma + 127) / 255;
        h ^= (b1 & 0xFFL);
        h *= FNV_PRIME;

        int s2 = sArr[base + 2] & 0xFF;
        int d2 = dArr[base + 2] & 0xFF;
        int b2 = (s2 * a + d2 * oma + 127) / 255;
        h ^= (b2 & 0xFFL);
        h *= FNV_PRIME;

        // Output alpha is always 255
        h ^= 255L;
        h *= FNV_PRIME;

        base += 4;
      }
      // As in baseline: return checksum of the last iteration only
      hash = h;
    }
    return hash;
  }
}
