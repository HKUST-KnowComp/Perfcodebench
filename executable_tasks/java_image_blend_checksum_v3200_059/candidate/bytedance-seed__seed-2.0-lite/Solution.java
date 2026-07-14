import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    byte[] outArray = new byte[src.length];
    long hash = 0;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        // Blend Red channel
        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        outArray[base] = (byte) (((long)(sR * a + dR * invA + 127) * 0x8081) >> 23);
        
        // Blend Green channel
        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        outArray[base + 1] = (byte) (((long)(sG * a + dG * invA + 127) * 0x8081) >> 23);
        
        // Blend Blue channel
        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        outArray[base + 2] = (byte) (((long)(sB * a + dB * invA + 127) * 0x8081) >> 23);
        
        // Output alpha is always 255
        outArray[base + 3] = (byte) 0xFF;
      }
      hash = checksumBytes(outArray);
    }
    return hash;
  }
}