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
        int invA = 255 - a;

        // Red
        int rS = src[i] & 0xFF;
        int rD = dst[i] & 0xFF;
        out[i] = (byte) ((rS * a + rD * invA + 127) / 255);

        // Green
        int gS = src[i + 1] & 0xFF;
        int gD = dst[i + 1] & 0xFF;
        out[i + 1] = (byte) ((gS * a + gD * invA + 127) / 255);

        // Blue
        int bS = src[i + 2] & 0xFF;
        int bD = dst[i + 2] & 0xFF;
        out[i + 2] = (byte) ((bS * a + bD * invA + 127) / 255);

        // Alpha
        out[i + 3] = (byte) 255;
      }
      
      // Inline checksumBytes to avoid extra method call and array copying
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