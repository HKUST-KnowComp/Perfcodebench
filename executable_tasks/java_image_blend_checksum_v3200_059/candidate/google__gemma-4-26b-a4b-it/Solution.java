import java.util.*;

public class Solution {
  // The original formula: (s * a + d * (255 - a) + 127) / 255
  // We must keep this exact logic to ensure the checksum matches.
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
    int totalBytes = width * height * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;
    int pixels = width * height;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        
        // Optimization: if alpha is 255, it's just src. If 0, it's just dst.
        // However, to guarantee exactness with the provided formula, we use the formula.
        out[base]     = (byte) blendByte(src[base]     & 0xFF, dst[base]     & 0xFF, a);
        out[base + 1] = (byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
        out[base + 2] = (byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}