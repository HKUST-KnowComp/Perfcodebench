import java.util.*;

public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int totalBytes = pixels * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        // Blend R
        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        out[base] = (byte) ((sR * a + dR * invA + 127) / 255);

        // Blend G
        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        out[base + 1] = (byte) ((sG * a + dG * invA + 127) / 255);

        // Blend B
        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        out[base + 2] = (byte) ((sB * a + dB * invA + 127) / 255);

        // Alpha is always 255
        out[base + 3] = (byte) 255;
      }
      
      // Calculate checksum for the current iteration
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