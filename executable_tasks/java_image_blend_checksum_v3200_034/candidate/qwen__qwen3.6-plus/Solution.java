import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        int r = (src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA;
        int g = (src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA;
        int b = (src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA;
        
        h ^= (r + 127) / 255;
        h *= 1099511628211L;
        h ^= (g + 127) / 255;
        h *= 1099511628211L;
        h ^= (b + 127) / 255;
        h *= 1099511628211L;
        h ^= 255L;
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}