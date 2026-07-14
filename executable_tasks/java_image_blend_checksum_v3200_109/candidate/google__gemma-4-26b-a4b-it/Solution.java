import java.util.*;

public class Solution {
  // The original formula: (s * a + d * (255 - a) + 127) / 255
  // This can be optimized using the property: (x + 127) / 255 ≈ (x + 128 + (x >> 8)) >> 8
  // However, to ensure exact correctness for the checksum, we use the integer math directly.
  // Since we are in a performance critical loop, we inline the math.

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
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
        int base = i << 2;
        int s0 = src[base] & 0xFF;
        int s1 = src[base + 1] & 0xFF;
        int s2 = src[base + 2] & 0xFF;
        int a  = src[base + 3] & 0xFF;

        int d0 = dst[base] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;

        // Exact implementation of (s * a + d * (255 - a) + 127) / 255
        int invA = 255 - a;
        out[base]     = (byte) ((s0 * a + d0 * invA + 127) / 255);
        out[base + 1] = (byte) ((s1 * a + d1 * invA + 127) / 255);
        out[base + 2] = (byte) ((s2 * a + d2 * invA + 127) / 255);
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}