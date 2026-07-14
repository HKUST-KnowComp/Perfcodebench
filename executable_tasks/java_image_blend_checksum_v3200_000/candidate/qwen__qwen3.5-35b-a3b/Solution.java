import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int size = src.length;
    byte[] out = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int r = blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
        int g = blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
        int b = blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
        out[base] = (byte) r;
        out[base + 1] = (byte) g;
        out[base + 2] = (byte) b;
        out[base + 3] = (byte) 255;
        if (iter == iters - 1) {
          if (i == 0) {
            hash = 1469598103934665603L;
          }
          hash ^= (r & 0xFFL);
          hash *= 1099511628211L;
          hash ^= (g & 0xFFL);
          hash *= 1099511628211L;
          hash ^= (b & 0xFFL);
          hash *= 1099511628211L;
          hash ^= 255L;
          hash *= 1099511628211L;
        }
      }
    }
    return hash;
  }
}