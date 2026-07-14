import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        
        byte r = (byte) (((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
        out[i] = r;
        h ^= (r & 0xFFL);
        h *= 1099511628211L;
        
        byte g = (byte) (((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255);
        out[i + 1] = g;
        h ^= (g & 0xFFL);
        h *= 1099511628211L;
        
        byte b = (byte) (((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255);
        out[i + 2] = b;
        h ^= (b & 0xFFL);
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