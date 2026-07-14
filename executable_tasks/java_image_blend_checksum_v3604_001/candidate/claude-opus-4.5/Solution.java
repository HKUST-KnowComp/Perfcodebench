import java.util.*;

public class Solution {
  private static final byte[][] BLEND_TABLE = new byte[256][256];
  
  static {
    for (int diff = 0; diff < 256; diff++) {
      for (int a = 0; a < 256; a++) {
        BLEND_TABLE[diff][a] = (byte)((diff * a + 127) / 255);
      }
    }
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int d0 = dst[base] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        int s0 = src[base] & 0xFF;
        int s1 = src[base + 1] & 0xFF;
        int s2 = src[base + 2] & 0xFF;
        out[base] = (byte)(d0 + BLEND_TABLE[s0 - d0 + 255][a] - BLEND_TABLE[255][a]);
        out[base + 1] = (byte)(d1 + BLEND_TABLE[s1 - d1 + 255][a] - BLEND_TABLE[255][a]);
        out[base + 2] = (byte)(d2 + BLEND_TABLE[s2 - d2 + 255][a] - BLEND_TABLE[255][a]);
        out[base + 3] = (byte)255;
      }
      
      hash = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        hash ^= (out[i] & 0xFFL);
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}