import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int totalBytes = pixels * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < totalBytes; i += 4) {
        int a = src[i + 3] & 0xFF;
        
        // Blend R, G, B
        for (int j = 0; j < 3; j++) {
          int s = src[i + j] & 0xFF;
          int d = dst[i + j] & 0xFF;
          out[i + j] = (byte) ((s * a + d * (255 - a) + 127) / 255);
        }
        // Alpha is always 255
        out[i + 3] = (byte) 255;
      }
      
      // Calculate checksum
      long currentHash = 1469598103934665603L;
      for (int i = 0; i < totalBytes; i++) {
        currentHash ^= (out[i] & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }
    return hash;
  }
}