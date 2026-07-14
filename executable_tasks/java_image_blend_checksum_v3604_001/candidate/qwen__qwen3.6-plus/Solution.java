import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 0;
    long fnvInit = 1469598103934665603L;
    long fnvPrime = 1099511628211L;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = fnvInit;
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int ia = 255 - a;
        
        int r = ( (src[base] & 0xFF) * a + (dst[base] & 0xFF) * ia + 127 ) / 255;
        int g = ( (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * ia + 127 ) / 255;
        int b = ( (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * ia + 127 ) / 255;
        
        h ^= (r & 0xFFL); h *= fnvPrime;
        h ^= (g & 0xFFL); h *= fnvPrime;
        h ^= (b & 0xFFL); h *= fnvPrime;
        h ^= 0xFFL; h *= fnvPrime;
      }
      hash = h;
    }
    return hash;
  }
}