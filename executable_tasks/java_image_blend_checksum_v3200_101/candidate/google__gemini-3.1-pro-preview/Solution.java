import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  /**
   * Optimized blend calculation for (s * a + d * (255 - a) + 127) / 255.
   * For the range [0, 65152], (v + 127) / 255 is equivalent to (v + 128 + ((v + 128) >> 8)) >> 8.
   */
  static int blendByte(int s, int d, int a) {
    int v = s * a + d * (255 - a) + 127;
    return (v + 1 + (v >> 8)) >> 8;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int totalBytes = width * height * 4;
    int pixels = width * height;
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        
        int r_s = src[base] & 0xFF;
        int g_s = src[base + 1] & 0xFF;
        int b_s = src[base + 2] & 0xFF;
        int a = src[base + 3] & 0xFF;

        int r_d = dst[base] & 0xFF;
        int g_d = dst[base + 1] & 0xFF;
        int b_d = dst[base + 2] & 0xFF;

        // Calculate blended components
        // Formula: (s * a + d * (255 - a) + 127) / 255
        int r_out = (r_s * a + r_d * (255 - a) + 127);
        r_out = (r_out + 1 + (r_out >> 8)) >> 8;
        
        int g_out = (g_s * a + g_d * (255 - a) + 127);
        g_out = (g_out + 1 + (g_out >> 8)) >> 8;
        
        int b_out = (b_s * a + b_d * (255 - a) + 127);
        b_out = (b_out + 1 + (b_out >> 8)) >> 8;

        // Update FNV-1a hash inline for R, G, B, and constant A=255
        h ^= (r_out & 0xFFL);
        h *= FNV_PRIME;
        h ^= (g_out & 0xFFL);
        h *= FNV_PRIME;
        h ^= (b_out & 0xFFL);
        h *= FNV_PRIME;
        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}