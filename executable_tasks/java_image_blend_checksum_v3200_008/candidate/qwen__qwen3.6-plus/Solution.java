public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;
    int len = (width * height) << 2;
    long hash = FNV_OFFSET;
    
    for (int i = 0; i < len; i += 4) {
      int a = src[i + 3] & 0xFF;
      int invA = 255 - a;
      
      int r = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255;
      int g = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255;
      int b = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255;
      
      hash ^= r; hash *= FNV_PRIME;
      hash ^= g; hash *= FNV_PRIME;
      hash ^= b; hash *= FNV_PRIME;
      hash ^= 255L; hash *= FNV_PRIME;
    }
    
    return hash;
  }
}