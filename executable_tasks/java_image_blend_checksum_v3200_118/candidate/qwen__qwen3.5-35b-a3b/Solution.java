import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    return ((s * a + d * (255 - a) + 127) * 257) >> 16;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        // R
        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        int blendedR = blendByte(sR, dR, a);
        hash ^= (blendedR & 0xFFL);
        hash *= 1099511628211L;
        // G
        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        int blendedG = blendByte(sG, dG, a);
        hash ^= (blendedG & 0xFFL);
        hash *= 1099511628211L;
        // B
        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        int blendedB = blendByte(sB, dB, a);
        hash ^= (blendedB & 0xFFL);
        hash *= 1099511628211L;
        // A
        hash ^= 255L;
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}