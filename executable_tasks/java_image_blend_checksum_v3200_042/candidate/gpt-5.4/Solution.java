import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int len = width * height * 4;
    final byte[] out = new byte[len];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int p = 0;
      for (int i = 0; i < len; i += 4) {
        final int a = src[i + 3] & 0xFF;
        final int invA = 255 - a;

        out[p++] = (byte) (((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
        out[p++] = (byte) (((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255);
        out[p++] = (byte) (((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255);
        out[p++] = (byte) 255;
      }

      long h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        h ^= (out[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }

    return hash;
  }
}
