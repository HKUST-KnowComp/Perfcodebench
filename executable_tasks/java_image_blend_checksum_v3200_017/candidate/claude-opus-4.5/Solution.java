import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int sr = src[base] & 0xFF;
        int dr = dst[base] & 0xFF;
        int r = (sr * a + dr * invA + 127) / 255;
        out[base] = (byte) r;
        hash ^= (r & 0xFFL);
        hash *= 1099511628211L;
        
        int sg = src[base + 1] & 0xFF;
        int dg = dst[base + 1] & 0xFF;
        int g = (sg * a + dg * invA + 127) / 255;
        out[base + 1] = (byte) g;
        hash ^= (g & 0xFFL);
        hash *= 1099511628211L;
        
        int sb = src[base + 2] & 0xFF;
        int db = dst[base + 2] & 0xFF;
        int b = (sb * a + db * invA + 127) / 255;
        out[base + 2] = (byte) b;
        hash ^= (b & 0xFFL);
        hash *= 1099511628211L;
        
        out[base + 3] = (byte) 255;
        hash ^= 255L;
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}