import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 1469598103934665603L;
    for (int i = 0; i < pixels; i++) {
      int base = i * 4;
      int sa = src[base] & 0xFF;
      int sg = src[base+1] & 0xFF;
      int sb = src[base+2] & 0xFF;
      int a = src[base+3] & 0xFF;
      int da = dst[base] & 0xFF;
      int dg = dst[base+1] & 0xFF;
      int db = dst[base+2] & 0xFF;
      int oneMinusA = 255 - a;
      int r = (sa * a + da * oneMinusA + 127) / 255;
      int g = (sg * a + dg * oneMinusA + 127) / 255;
      int b = (sb * a + db * oneMinusA + 127) / 255;
      hash ^= (r & 0xFFL);
      hash *= 1099511628211L;
      hash ^= (g & 0xFFL);
      hash *= 1099511628211L;
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
      hash ^= 255L;
      hash *= 1099511628211L;
    }
    return hash;
  }
}