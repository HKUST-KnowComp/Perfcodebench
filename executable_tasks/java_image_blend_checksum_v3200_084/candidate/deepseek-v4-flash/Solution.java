import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int length = pixels * 4;
    byte[] output = new byte[length];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        // Red
        int r = (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127;
        output[base] = (byte) (r / 255);
        // Green
        int g = (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127;
        output[base + 1] = (byte) (g / 255);
        // Blue
        int b = (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127;
        output[base + 2] = (byte) (b / 255);
        // Alpha always 255
        output[base + 3] = (byte) 255;
      }
      hash = checksumBytes(output);
    }
    return hash;
  }
}