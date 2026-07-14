import java.util.*;

public class Solution {
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
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;

        // Optimized blend: (s * a + d * (255 - a) + 127) / 255
        out[base]     = (byte) (( (src[base]     & 0xFF) * a + (dst[base]     & 0xFF) * invA + 127) / 255);
        out[base + 1] = (byte) (( (src[base + 1] & 0xFF) * a + (dst[base + 1] & 0xFF) * invA + 127) / 255);
        out[base + 2] = (byte) (( (src[base + 2] & 0xFF) * a + (dst[base + 2] & 0xFF) * invA + 127) / 255);
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}