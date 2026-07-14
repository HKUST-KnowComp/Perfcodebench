import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < len; i += 4) {
        int sa = src[i + 3] & 0xFF;
        int da = 255 - sa;
        
        int b0 = ((src[i] & 0xFF) * sa + (dst[i] & 0xFF) * da + 127) / 255;
        int b1 = ((src[i + 1] & 0xFF) * sa + (dst[i + 1] & 0xFF) * da + 127) / 255;
        int b2 = ((src[i + 2] & 0xFF) * sa + (dst[i + 2] & 0xFF) * da + 127) / 255;
        int b3 = 255;
        
        hash ^= (b0 & 0xFFL); hash *= 1099511628211L;
        hash ^= (b1 & 0xFFL); hash *= 1099511628211L;
        hash ^= (b2 & 0xFFL); hash *= 1099511628211L;
        hash ^= (b3 & 0xFFL); hash *= 1099511628211L;
      }
    }
    return hash;
  }
}