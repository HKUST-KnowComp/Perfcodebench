import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;
    int pixels = width * height;
    byte[] out = new byte[pixels * 4];
    for (int i = 0; i < pixels; i++) {
      int base = i * 4;
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;
      out[base]     = (byte) (((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255);
      out[base + 1] = (byte) (((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255);
      out[base + 2] = (byte) (((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255);
      out[base + 3] = (byte) 255;
    }
    return checksumBytes(out);
  }
}