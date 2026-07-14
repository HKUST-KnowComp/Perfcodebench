import java.util.*;

public class Solution {
  private static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = data.length; i < n; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int len = width * height * 4;
    final byte[] out = new byte[len];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i += 4) {
        final int a = src[i + 3] & 0xFF;
        final int invA = 255 - a;

        final int sr = src[i] & 0xFF;
        final int sg = src[i + 1] & 0xFF;
        final int sb = src[i + 2] & 0xFF;

        final int dr = dst[i] & 0xFF;
        final int dg = dst[i + 1] & 0xFF;
        final int db = dst[i + 2] & 0xFF;

        out[i] = (byte) ((sr * a + dr * invA + 127) / 255);
        out[i + 1] = (byte) ((sg * a + dg * invA + 127) / 255);
        out[i + 2] = (byte) ((sb * a + db * invA + 127) / 255);
        out[i + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}
