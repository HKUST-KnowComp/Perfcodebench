import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int totalBytes = pixels * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < totalBytes; i += 4) {
        int sR = src[i] & 0xFF;
        int sG = src[i + 1] & 0xFF;
        int sB = src[i + 2] & 0xFF;
        int a = src[i + 3] & 0xFF;
        
        int dR = dst[i] & 0xFF;
        int dG = dst[i + 1] & 0xFF;
        int dB = dst[i + 2] & 0xFF;

        int invA = 255 - a;
        
        out[i] = (byte) ((sR * a + dR * invA + 127) / 255);
        out[i + 1] = (byte) ((sG * a + dG * invA + 127) / 255);
        out[i + 2] = (byte) ((sB * a + dB * invA + 127) / 255);
        out[i + 3] = (byte) 255;
      }
      
      // Inline checksumBytes
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