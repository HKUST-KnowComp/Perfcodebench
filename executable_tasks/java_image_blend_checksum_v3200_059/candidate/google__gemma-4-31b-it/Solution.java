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
        out[base] = (byte) (( (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255);
        // Blend G
        out[base + 1] = (byte) (( (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255);
        // Blend B
        out[base + 2] = (byte) (( (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255);
        // Alpha is always 255
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}