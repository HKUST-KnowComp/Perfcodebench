import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    byte[] out = new byte[pixels * 4];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        out[base] = (byte) blendByte(sR, dR, a);
        out[base + 1] = (byte) blendByte(sG, dG, a);
        out[base + 2] = (byte) blendByte(sB, dB, a);
        out[base + 3] = 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}