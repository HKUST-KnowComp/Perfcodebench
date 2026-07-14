import java.util.*;

public class Solution {
  /**
   * Optimized blend function.
   * The original formula: (s * a + d * (255 - a) + 127) / 255
   * This is mathematically equivalent to a weighted average.
   */
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int totalBytes = width * height * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < totalBytes; i += 4) {
        int sR = src[i] & 0xFF;
        int sG = src[i + 1] & 0xFF;
        int sB = src[i + 2] & 0xFF;
        int a  = src[i + 3] & 0xFF;

        int dR = dst[i] & 0xFF;
        int dG = dst[i + 1] & 0xFF;
        int dB = dst[i + 2] & 0xFF;

        // Manual inlining of blendByte for performance
        out[i]     = (byte) ((sR * a + dR * (255 - a) + 127) / 255);
        out[i + 1] = (byte) ((sG * a + dG * (255 - a) + 127) / 255);
        out[i + 2] = (byte) ((sB * a + dB * (255 - a) + 127) / 255);
        out[i + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}