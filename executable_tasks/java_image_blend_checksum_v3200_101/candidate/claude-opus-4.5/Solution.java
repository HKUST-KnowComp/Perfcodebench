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
        out[base] = (byte) ((((src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA) + 127) / 255);
        out[base + 1] = (byte) ((((src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA) + 127) / 255);
        out[base + 2] = (byte) ((((src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA) + 127) / 255);
        out[base + 3] = (byte) 255;
      }
      
      long h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        h ^= (out[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}