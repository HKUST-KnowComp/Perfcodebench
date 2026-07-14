public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0;
    long hash = 1469598103934665603L;
    int pixels = width * height;
    for (int i = 0; i < pixels; i++) {
      int base = i << 2;
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;
      
      int r = (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA;
      hash ^= ((r + 127) / 255) & 0xFFL;
      hash *= 1099511628211L;
      
      int g = (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA;
      hash ^= ((g + 127) / 255) & 0xFFL;
      hash *= 1099511628211L;
      
      int b = (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA;
      hash ^= ((b + 127) / 255) & 0xFFL;
      hash *= 1099511628211L;
      
      hash ^= 255L;
      hash *= 1099511628211L;
    }
    return hash;
  }
}