import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int sr = src[base] & 0xFF;
        int dr = dst[base] & 0xFF;
        out[base] = (byte) ((sr * a + dr * invA + 127) / 255);
        
        int sg = src[base + 1] & 0xFF;
        int dg = dst[base + 1] & 0xFF;
        out[base + 1] = (byte) ((sg * a + dg * invA + 127) / 255);
        
        int sb = src[base + 2] & 0xFF;
        int db = dst[base + 2] & 0xFF;
        out[base + 2] = (byte) ((sb * a + db * invA + 127) / 255);
        
        out[base + 3] = (byte) 255;
      }
      
      hash = 1469598103934665603L;
      int i = 0;
      int limit = len - 3;
      while (i < limit) {
        hash ^= (out[i] & 0xFFL);
        hash *= 1099511628211L;
        hash ^= (out[i + 1] & 0xFFL);
        hash *= 1099511628211L;
        hash ^= (out[i + 2] & 0xFFL);
        hash *= 1099511628211L;
        hash ^= (out[i + 3] & 0xFFL);
        hash *= 1099511628211L;
        i += 4;
      }
      while (i < len) {
        hash ^= (out[i] & 0xFFL);
        hash *= 1099511628211L;
        i++;
      }
    }
    return hash;
  }
}