public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int outLen = pixels * 4;
    byte[] out = new byte[outLen];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      int base = 0;
      for (int i = 0; i < pixels; i++) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        int s0 = src[base] & 0xFF;
        int s1 = src[base + 1] & 0xFF;
        int s2 = src[base + 2] & 0xFF;
        int d0 = dst[base] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        
        out[idx++] = (byte) ((s0 * a + d0 * invA + 127) / 255);
        out[idx++] = (byte) ((s1 * a + d1 * invA + 127) / 255);
        out[idx++] = (byte) ((s2 * a + d2 * invA + 127) / 255);
        out[idx++] = (byte) 255;
        base += 4;
      }
      long h = 1469598103934665603L;
      for (int i = 0; i < outLen; i++) {
        h ^= (out[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}