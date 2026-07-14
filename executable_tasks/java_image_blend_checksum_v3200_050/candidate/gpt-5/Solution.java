import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    long hash = 0L;
    final byte[] s = src;
    final byte[] d = dst;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV64_OFFSET;
      int base = 0;
      for (int i = 0; i < pixels; i++) {
        final int a = s[base + 3] & 0xFF;
        final int ia = 255 - a;

        int sr = s[base] & 0xFF;
        int dr = d[base] & 0xFF;
        int r = (sr * a + dr * ia + 127) / 255;
        h ^= (long) (r & 0xFF);
        h *= FNV64_PRIME;

        int sg = s[base + 1] & 0xFF;
        int dg = d[base + 1] & 0xFF;
        int g = (sg * a + dg * ia + 127) / 255;
        h ^= (long) (g & 0xFF);
        h *= FNV64_PRIME;

        int sb = s[base + 2] & 0xFF;
        int db = d[base + 2] & 0xFF;
        int b = (sb * a + db * ia + 127) / 255;
        h ^= (long) (b & 0xFF);
        h *= FNV64_PRIME;

        // Alpha channel in output is 255
        h ^= 255L;
        h *= FNV64_PRIME;

        base += 4;
      }
      hash = h; // Only the last iteration's checksum is returned
    }

    return hash;
  }
}
