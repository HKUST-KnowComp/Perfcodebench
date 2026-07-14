import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  static long checksumBytes(byte[] data) {
    long hash = FNV_OFFSET_BASIS;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int len = pixels << 2;
    final byte[] out = new byte[len];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      int si = 0;

      for (int p = 0; p < pixels; p++) {
        final int s0 = src[si] & 0xFF;
        final int s1 = src[si + 1] & 0xFF;
        final int s2 = src[si + 2] & 0xFF;
        final int a = src[si + 3] & 0xFF;

        final int invA = 255 - a;
        final int o0 = (s0 * a + (dst[si] & 0xFF) * invA + 127) / 255;
        final int o1 = (s1 * a + (dst[si + 1] & 0xFF) * invA + 127) / 255;
        final int o2 = (s2 * a + (dst[si + 2] & 0xFF) * invA + 127) / 255;

        out[si] = (byte) o0;
        h ^= (o0 & 0xFFL);
        h *= FNV_PRIME;

        out[si + 1] = (byte) o1;
        h ^= (o1 & 0xFFL);
        h *= FNV_PRIME;

        out[si + 2] = (byte) o2;
        h ^= (o2 & 0xFFL);
        h *= FNV_PRIME;

        out[si + 3] = (byte) 255;
        h ^= 255L;
        h *= FNV_PRIME;

        si += 4;
      }

      hash = h;
    }

    return hash;
  }
}
