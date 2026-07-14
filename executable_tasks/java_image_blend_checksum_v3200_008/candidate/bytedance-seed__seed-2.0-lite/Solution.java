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
    byte[] out = new byte[src.length];
    long hash = 0;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int sR = src[base] & 0xFF;
        int sG = src[base + 1] & 0xFF;
        int sB = src[base + 2] & 0xFF;
        int dR = dst[base] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        
        int oR = (sR * a + dR * (255 - a) + 127) / 255;
        int oG = (sG * a + dG * (255 - a) + 127) / 255;
        int oB = (sB * a + dB * (255 - a) + 127) / 255;
        
        out[base] = (byte) oR;
        out[base + 1] = (byte) oG;
        out[base + 2] = (byte) oB;
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}