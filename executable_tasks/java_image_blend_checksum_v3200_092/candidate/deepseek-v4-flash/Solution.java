public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int totalBytes = pixels * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        int r = ( (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        int g = ( (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        int b = ( (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        out[base] = (byte) r;
        out[base + 1] = (byte) g;
        out[base + 2] = (byte) b;
        out[base + 3] = (byte) 255;
        h ^= (out[base] & 0xFFL);
        h *= 1099511628211L;
        h ^= (out[base + 1] & 0xFFL);
        h *= 1099511628211L;
        h ^= (out[base + 2] & 0xFFL);
        h *= 1099511628211L;
        h ^= (out[base + 3] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}