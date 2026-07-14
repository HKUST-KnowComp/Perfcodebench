import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = src.length;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        
        int r = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255;
        hash ^= r;
        hash *= 1099511628211L;
        
        int g = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255;
        hash ^= g;
        hash *= 1099511628211L;
        
        int b = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255;
        hash ^= b;
        hash *= 1099511628211L;
        
        hash ^= 255;
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}