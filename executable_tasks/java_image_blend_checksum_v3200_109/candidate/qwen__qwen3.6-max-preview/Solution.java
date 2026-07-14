import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;
    int pixels = width * height;
    long hash = 1469598103934665603L;
    long prime = 1099511628211L;
    int len = pixels * 4;
    
    for (int i = 0; i < len; i += 4) {
      int a = src[i + 3] & 0xFF;
      int invA = 255 - a;
      
      int r = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255;
      int g = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255;
      int b = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255;
      
      hash ^= r; hash *= prime;
      hash ^= g; hash *= prime;
      hash ^= b; hash *= prime;
      hash ^= 255; hash *= prime;
    }
    return hash;
  }
}