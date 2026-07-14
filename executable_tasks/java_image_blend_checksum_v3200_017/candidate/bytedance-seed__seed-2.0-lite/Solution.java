import java.util.*;

public class Solution {
  private static final long FNV_PRIME = 1099511628211L;
  private static final long FNV_OFFSET = 1469598103934665603L;

  static long checksumBytes(byte[] data) {
    long hash = FNV_OFFSET;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int totalPixels = width * height;
    final int totalBytes = totalPixels * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < totalPixels; i++) {
        final int base = i * 4;
        final int a = src[base + 3] & 0xFF;
        final int invA = 255 - a;

        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        int sumR = sR * a + dR * invA + 127;
        out[base] = (byte) ((sumR * 0x8081) >> 23);

        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        int sumG = sG * a + dG * invA + 127;
        out[base + 1] = (byte) ((sumG * 0x8081) >> 23);

        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        int sumB = sB * a + dB * invA + 127;
        out[base + 2] = (byte) ((sumB * 0x8081) >> 23);

        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}