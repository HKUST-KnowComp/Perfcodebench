import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    long hash = 0;
    int len = width * height * 4;
    long FNV_SEED = 1469598103934665603L;
    long FNV_PRIME = 1099511628211L;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_SEED;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int ia = 255 - a;
        
        int s0 = src[i] & 0xFF;
        int s1 = src[i + 1] & 0xFF;
        int s2 = src[i + 2] & 0xFF;
        
        int d0 = dst[i] & 0xFF;
        int d1 = dst[i + 1] & 0xFF;
        int d2 = dst[i + 2] & 0xFF;
        
        int r = (s0 * a + d0 * ia + 127) / 255;
        int g = (s1 * a + d1 * ia + 127) / 255;
        int b = (s2 * a + d2 * ia + 127) / 255;
        
        h ^= r; h *= FNV_PRIME;
        h ^= g; h *= FNV_PRIME;
        h ^= b; h *= FNV_PRIME;
        h ^= 255; h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}