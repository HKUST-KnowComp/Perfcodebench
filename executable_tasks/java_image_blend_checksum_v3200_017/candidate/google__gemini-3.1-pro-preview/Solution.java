import java.util.*;

public class Solution {
  /**
   * Optimized blend calculation: (s * a + d * (255 - a) + 127) / 255
   * Using the identity (x + (x >> 8) + 1) >> 8 is not quite right for the +127 offset,
   * but (t + (t >> 8)) >> 8 is a very close approximation for t/255.
   * Given the range of t (0 to 65152), (t + (t >> 8)) >> 8 is exactly equal to t/255.
   */
  static final int blend(int s, int d, int a) {
    int t = s * a + d * (255 - a) + 127;
    return (t + (t >> 8)) >> 8;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int totalBytes = width * height * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      long currentHash = FNV_OFFSET_BASIS;
      
      for (int i = 0; i < totalBytes; i += 4) {
        int r_s = src[i] & 0xFF;
        int g_s = src[i + 1] & 0xFF;
        int b_s = src[i + 2] & 0xFF;
        int a_s = src[i + 3] & 0xFF;

        int r_d = dst[i] & 0xFF;
        int g_d = dst[i + 1] & 0xFF;
        int b_d = dst[i + 2] & 0xFF;

        // Calculate blended components
        int r_out = blend(r_s, r_d, a_s);
        int g_out = blend(g_s, g_d, a_s);
        int b_out = blend(b_s, b_d, a_s);
        int a_out = 255;

        // Update hash inline to avoid second pass
        currentHash ^= (r_out & 0xFFL);
        currentHash *= FNV_PRIME;
        currentHash ^= (g_out & 0xFFL);
        currentHash *= FNV_PRIME;
        currentHash ^= (b_out & 0xFFL);
        currentHash *= FNV_PRIME;
        currentHash ^= (a_out & 0xFFL);
        currentHash *= FNV_PRIME;
      }
      hash = currentHash;
    }
    return hash;
  }
}