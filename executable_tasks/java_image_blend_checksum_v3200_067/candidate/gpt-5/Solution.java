import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int totalBytes = pixels * 4;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    long hash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      int base = 0;
      while (base < totalBytes) {
        int a = src[base + 3] & 0xFF;
        int inv = 255 - a;

        int s = src[base] & 0xFF;
        int d = dst[base] & 0xFF;
        int v = (s * a + d * inv + 127) / 255;
        h ^= (v & 0xFFL);
        h *= FNV_PRIME;

        s = src[base + 1] & 0xFF;
        d = dst[base + 1] & 0xFF;
        v = (s * a + d * inv + 127) / 255;
        h ^= (v & 0xFFL);
        h *= FNV_PRIME;

        s = src[base + 2] & 0xFF;
        d = dst[base + 2] & 0xFF;
        v = (s * a + d * inv + 127) / 255;
        h ^= (v & 0xFFL);
        h *= FNV_PRIME;

        // Alpha channel is fully opaque in output
        h ^= 255L;
        h *= FNV_PRIME;

        base += 4;
      }
      hash = h;
    }
    return hash;
  }
}
