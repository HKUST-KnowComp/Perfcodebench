import java.util.*;

public class Solution {
  private static final int[] BLEND_LOOKUP = new int[2 * 255 * 255 + 1];
  static {
    for (int x = 0; x < BLEND_LOOKUP.length; x++) {
      BLEND_LOOKUP[x] = (x + 127) / 255;
    }
  }

  static int blendByte(int s, int d, int a) {
    return BLEND_LOOKUP[s * a + d * (255 - a)];
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    byte[] outArray = new byte[src.length];
    long hash = 0;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        outArray[base] = (byte) blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
        outArray[base + 1] = (byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
        outArray[base + 2] = (byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
        outArray[base + 3] = (byte) 255;
      }
      hash = checksumBytes(outArray);
    }
    return hash;
  }
}