import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int len = width * height * 4;
    final byte[] out = new byte[len];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int i = 0;
      while (i < len) {
        final int a = src[i + 3] & 0xFF;
        final int invA = 255 - a;

        out[i] = (byte) (((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
        out[i + 1] = (byte) (((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255);
        out[i + 2] = (byte) (((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255);
        out[i + 3] = (byte) 255;
        i += 4;
      }

      long h = 1469598103934665603L;
      for (int j = 0; j < len; j++) {
        h ^= (out[j] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }

    return hash;
  }
}
