public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int s0 = src[base] & 0xFF;
        int s1 = src[base + 1] & 0xFF;
        int s2 = src[base + 2] & 0xFF;
        int d0 = dst[base] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        
        int r = s0 * a + d0 * invA;
        int g = s1 * a + d1 * invA;
        int b = s2 * a + d2 * invA;
        
        hash ^= ((r + 127) / 255) & 0xFFL;
        hash *= 1099511628211L;
        hash ^= ((g + 127) / 255) & 0xFFL;
        hash *= 1099511628211L;
        hash ^= ((b + 127) / 255) & 0xFFL;
        hash *= 1099511628211L;
        hash ^= 255L;
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}