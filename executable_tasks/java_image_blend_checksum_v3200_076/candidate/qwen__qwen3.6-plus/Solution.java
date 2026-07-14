import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    long hash = 0;
    final long FNV_PRIME = 1099511628211L;
    final long FNV_OFFSET = 1469598103934665603L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        
        int r = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255;
        int g = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255;
        int b = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255;
        
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