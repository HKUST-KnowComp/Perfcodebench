import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int base = 0; base < len; base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int r = ((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255;
        h ^= r;
        h *= 1099511628211L;
        
        int g = ((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255;
        h ^= g;
        h *= 1099511628211L;
        
        int b = ((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255;
        h ^= b;
        h *= 1099511628211L;
        
        h ^= 255L;
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}
