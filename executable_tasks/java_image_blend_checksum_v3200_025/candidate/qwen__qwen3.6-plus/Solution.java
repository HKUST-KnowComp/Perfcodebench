import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int ia = 255 - a;
        
        int s0 = src[i] & 0xFF;
        int d0 = dst[i] & 0xFF;
        out[i] = (byte) ((s0 * a + d0 * ia + 127) / 255);
        
        int s1 = src[i + 1] & 0xFF;
        int d1 = dst[i + 1] & 0xFF;
        out[i + 1] = (byte) ((s1 * a + d1 * ia + 127) / 255);
        
        int s2 = src[i + 2] & 0xFF;
        int d2 = dst[i + 2] & 0xFF;
        out[i + 2] = (byte) ((s2 * a + d2 * ia + 127) / 255);
        
        out[i + 3] = (byte) 255;
      }
      
      long h = 1469598103934665603L;
      for (int j = 0; j < len; j++) {
        h ^= (out[j] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}