import java.util.*;

public class Solution {
  // The original formula: (s * a + d * (255 - a) + 127) / 255
  // This is mathematically equivalent to: s*a + d*(255-a) + 127 / 255
  // We must keep this exact logic for correctness.
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
        
        // Pre-calculate (255 - a) to save operations
        int invA = 255 - a;
        
        // R
        int rS = src[base] & 0xFF;
        int rD = dst[base] & 0xFF;
        out[base] = (byte) ((rS * a + rD * invA + 127) / 255);
        
        // G
        int gS = src[base + 1] & 0xFF;
        int gD = dst[base + 1] & 0xFF;
        out[base + 1] = (byte) ((gS * a + gD * invA + 127) / 255);
        
        // B
        int bS = src[base + 2] & 0xFF;
        int bD = dst[base + 2] & 0xFF;
        out[base + 2] = (byte) ((bS * a + bD * invA + 127) / 255);
        
        // A
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}