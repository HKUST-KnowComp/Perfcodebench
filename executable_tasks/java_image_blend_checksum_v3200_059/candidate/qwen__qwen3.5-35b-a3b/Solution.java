import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
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
    int pixels = width * height;
    int size = pixels * 4;
    byte[] out = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        out[base] = (byte) blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
        out[base + 1] = (byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
        out[base + 2] = (byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
        out[base + 3] = 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}