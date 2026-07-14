public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = src.length;
    long hash = 0;
    final long PRIME = 1099511628211L;
    final long INIT = 1469598103934665603L;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = INIT;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int ia = 255 - a;
        
        int r = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * ia + 127) / 255;
        int g = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * ia + 127) / 255;
        int b = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * ia + 127) / 255;
        
        h ^= r;
        h *= PRIME;
        h ^= g;
        h *= PRIME;
        h ^= b;
        h *= PRIME;
        h ^= 0xFFL;
        h *= PRIME;
      }
      hash = h;
    }
    return hash;
  }
}