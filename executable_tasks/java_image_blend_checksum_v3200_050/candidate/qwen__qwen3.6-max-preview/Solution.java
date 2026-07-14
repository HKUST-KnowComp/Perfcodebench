import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        
        int v0 = ((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255;
        out[i] = (byte) v0;
        h ^= (v0 & 0xFFL);
        h *= 1099511628211L;
        
        int v1 = ((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255;
        out[i + 1] = (byte) v1;
        h ^= (v1 & 0xFFL);
        h *= 1099511628211L;
        
        int v2 = ((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255;
        out[i + 2] = (byte) v2;
        h ^= (v2 & 0xFFL);
        h *= 1099511628211L;
        
        out[i + 3] = (byte) 255;
        h ^= 255L;
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}