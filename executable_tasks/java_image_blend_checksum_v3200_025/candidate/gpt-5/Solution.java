import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    // Match baseline behavior when iters <= 0
    if (iters <= 0) return 0L;

    final int pixels = width * height;
    final int end = pixels << 2; // total bytes in RGBA stream

    // FNV-1 64-bit constants
    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    long hash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int i = 0; i < end; i += 4) {
        int a = src[i + 3] & 0xFF;
        int m = 255 - a;

        int s = src[i] & 0xFF;
        int d = dst[i] & 0xFF;
        int r = (s * a + d * m + 127) / 255;
        h ^= (r & 0xFFL);
        h *= FNV_PRIME;

        s = src[i + 1] & 0xFF;
        d = dst[i + 1] & 0xFF;
        int g = (s * a + d * m + 127) / 255;
        h ^= (g & 0xFFL);
        h *= FNV_PRIME;

        s = src[i + 2] & 0xFF;
        d = dst[i + 2] & 0xFF;
        int b = (s * a + d * m + 127) / 255;
        h ^= (b & 0xFFL);
        h *= FNV_PRIME;

        // Alpha channel is always 255 in output
        h ^= 255L;
        h *= FNV_PRIME;
      }
      // As in baseline, keep the checksum of the last iteration
      hash = h;
    }
    return hash;
  }
}
