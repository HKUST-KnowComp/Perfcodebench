import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters == 0) return 0;
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    int pixels = width * height;
    
    for (int i = 0; i < pixels; i++) {
      int base = i << 2;
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;
      
      int r = (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA;
      int g = (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA;
      int b = (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA;
      
      int outR = (r + 127) / 255;
      int outG = (g + 127) / 255;
      int outB = (b + 127) / 255;
      
      hash ^= outR; hash *= FNV_PRIME;
      hash ^= outG; hash *= FNV_PRIME;
      hash ^= outB; hash *= FNV_PRIME;
      hash ^= 255; hash *= FNV_PRIME;
    }
    return hash;
  }
}