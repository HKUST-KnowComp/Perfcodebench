import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    long hash = 0;
    final long fnvOffset = 1469598103934665603L;
    final long fnvMult = 1099511628211L;
    for (int iter = 0; iter < iters; iter++) {
      long h = fnvOffset;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        // red
        int s = src[base] & 0xFF;
        int d = dst[base] & 0xFF;
        int r = (s * a + d * invA + 127) / 255;
        h ^= (r & 0xFFL);
        h *= fnvMult;
        // green
        s = src[base + 1] & 0xFF;
        d = dst[base + 1] & 0xFF;
        int g = (s * a + d * invA + 127) / 255;
        h ^= (g & 0xFFL);
        h *= fnvMult;
        // blue
        s = src[base + 2] & 0xFF;
        d = dst[base + 2] & 0xFF;
        int b = (s * a + d * invA + 127) / 255;
        h ^= (b & 0xFFL);
        h *= fnvMult;
        // alpha = 255
        h ^= 255L;
        h *= fnvMult;
      }
      hash = h;
    }
    return hash;
  }
}