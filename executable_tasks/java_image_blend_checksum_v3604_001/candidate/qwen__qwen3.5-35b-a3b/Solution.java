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
        int s_r = src[base] & 0xFF;
        int s_g = src[base + 1] & 0xFF;
        int s_b = src[base + 2] & 0xFF;
        int d_r = dst[base] & 0xFF;
        int d_g = dst[base + 1] & 0xFF;
        int d_b = dst[base + 2] & 0xFF;

        out[base] = (byte) blendByte(s_r, d_r, a);
        out[base + 1] = (byte) blendByte(s_g, d_g, a);
        out[base + 2] = (byte) blendByte(s_b, d_b, a);
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}