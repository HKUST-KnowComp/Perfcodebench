import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int size = pixels * 4;
    byte[] out = new byte[size];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        out[idx++] = (byte) ((((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255));
        out[idx++] = (byte) ((((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255));
        out[idx++] = (byte) ((((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255));
        out[idx++] = (byte) 255;
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