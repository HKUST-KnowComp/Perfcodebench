import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int size = pixels * 4;
    byte[] out = new byte[size];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int s0 = src[base] & 0xFF;
        int d0 = dst[base] & 0xFF;
        out[base] = (byte) ((s0 * a + d0 * invA + 127) / 255);
        
        int s1 = src[base + 1] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        out[base + 1] = (byte) ((s1 * a + d1 * invA + 127) / 255);
        
        int s2 = src[base + 2] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        out[base + 2] = (byte) ((s2 * a + d2 * invA + 127) / 255);
        
        out[base + 3] = (byte) 255;
      }
      
      hash = 1469598103934665603L;
      for (int i = 0; i < size; i++) {
        hash ^= (out[i] & 0xFFL);
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}