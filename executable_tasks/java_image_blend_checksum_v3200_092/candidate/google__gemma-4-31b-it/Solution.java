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

        // Blend R, G, B
        // (s * a + d * (255 - a) + 127) / 255
        out[i] = (byte) (( (src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
        out[i + 1] = (byte) (( (src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255);
        out[i + 2] = (byte) (( (src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255);
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