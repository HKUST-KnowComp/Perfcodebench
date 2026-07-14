import java.util.*;

public class Solution {
  /**
   * Optimized blend function using the formula: (s * a + d * (255 - a) + 127) / 255
   * This can be approximated or calculated exactly. To match the baseline exactly:
   * (s * a + d * 255 - d * a + 127) / 255
   */
  private static int blend(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int totalBytes = width * height * 4;
    byte[] resultBuffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      // Process pixels
      for (int i = 0; i < totalBytes; i += 4) {
        int rS = src[i] & 0xFF;
        int gS = src[i + 1] & 0xFF;
        int bS = src[i + 2] & 0xFF;
        int aS = src[i + 3] & 0xFF;

        int rD = dst[i] & 0xFF;
        int gD = dst[i + 1] & 0xFF;
        int bD = dst[i + 2] & 0xFF;

        resultBuffer[i]     = (byte) ((rS * aS + rD * (255 - aS) + 127) / 255);
        resultBuffer[i + 1] = (byte) ((gS * aS + gD * (255 - aS) + 127) / 255);
        resultBuffer[i + 2] = (byte) ((bS * aS + bD * (255 - aS) + 127) / 255);
        resultBuffer[i + 3] = (byte) 255;
      }

      // FNV-1a 64-bit hash
      long currentHash = 1469598103934665603L;
      for (int i = 0; i < totalBytes; i++) {
        currentHash ^= (resultBuffer[i] & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }

    return hash;
  }
}