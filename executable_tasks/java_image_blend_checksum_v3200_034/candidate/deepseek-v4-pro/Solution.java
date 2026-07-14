public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters == 0) return 0L;
    int pixels = width * height;
    byte[] out = new byte[pixels * 4];
    byte[] ls = src;
    byte[] ld = dst;
    for (int i = 0, base = 0; i < pixels; i++, base += 4) {
      int sa = ls[base + 3] & 0xFF;
      int invA = 255 - sa;
      int r = ((ls[base] & 0xFF) * sa + (ld[base] & 0xFF) * invA + 127) / 255;
      int g = ((ls[base + 1] & 0xFF) * sa + (ld[base + 1] & 0xFF) * invA + 127) / 255;
      int b = ((ls[base + 2] & 0xFF) * sa + (ld[base + 2] & 0xFF) * invA + 127) / 255;
      out[base] = (byte) r;
      out[base + 1] = (byte) g;
      out[base + 2] = (byte) b;
      out[base + 3] = (byte) 255;
    }
    long hash = 1469598103934665603L;
    for (byte val : out) {
      hash ^= (val & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }
}