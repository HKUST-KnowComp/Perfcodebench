import java.util.*;

public class Solution {
  /**
   * Optimized blend function: (s * a + d * (255 - a) + 127) / 255
   * This specific integer arithmetic is common in graphics.
   * We can optimize (x + 127) / 255 as ((x + 128) + (x + 128) >> 8) >> 8 for some cases,
   * but to ensure exact correctness with the baseline, we use the standard integer division
   * or a very close bit-shift approximation. 
   * Given the performance goal, we'll inline the math.
   */
  
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    byte[] out = new byte[len];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      // Process pixels
      for (int i = 0; i < len; i += 4) {
        int r_s = src[i] & 0xFF;
        int g_s = src[i + 1] & 0xFF;
        int b_s = src[i + 2] & 0xFF;
        int a   = src[i + 3] & 0xFF;

        int r_d = dst[i] & 0xFF;
        int g_d = dst[i + 1] & 0xFF;
        int b_d = dst[i + 2] & 0xFF;

        // Exact formula: (s * a + d * (255 - a) + 127) / 255
        int invA = 255 - a;
        out[i]     = (byte) ((r_s * a + r_d * invA + 127) / 255);
        out[i + 1] = (byte) ((g_s * a + g_d * invA + 127) / 255);
        out[i + 2] = (byte) ((b_s * a + b_d * invA + 127) / 255);
        out[i + 3] = (byte) 255;
      }

      // FNV-1a hash calculation
      long h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        h ^= (out[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    
    return hash;
  }
}