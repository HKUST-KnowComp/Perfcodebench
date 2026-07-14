public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int inv = 255 - a;
        int r = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * inv + 127) / 255;
        h ^= (r & 0xFFL);
        h *= 1099511628211L;
        int g = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * inv + 127) / 255;
        h ^= (g & 0xFFL);
        h *= 1099511628211L;
        int b = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * inv + 127) / 255;
        h ^= (b & 0xFFL);
        h *= 1099511628211L;
        h ^= 255L;
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}